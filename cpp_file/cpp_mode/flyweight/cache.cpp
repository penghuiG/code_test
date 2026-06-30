/**
 * 缓存模式 (Cache Pattern)
 * 
 * 缓存模式通过将频繁访问的数据存储在高速存储介质中，以减少对低速数据源
 * 的访问次数，从而提高系统性能。缓存模式的核心思想是"用空间换时间"。
 * 
 * 本例以网页资源缓存和数据库查询缓存为场景，演示两种常用的缓存淘汰策略：
 *   - LRU Cache（最近最少使用）：淘汰最长时间未被访问的数据
 *   - LFU Cache（最不经常使用）：淘汰访问频率最低的数据
 * 
 * 核心结构：
 *   - Cache（缓存接口）：定义缓存的通用操作（get、put、remove、clear等）
 *   - LRUCache（LRU缓存）：基于双向链表 + 哈希表实现，O(1) 时间复杂度
 *   - LFUCache（LFU缓存）：基于频率链表 + 哈希表实现，O(1) 时间复杂度
 *   - CacheManager（缓存管理器）：外观类，管理多个缓存实例
 * 
 * 应用场景：
 *   - 数据库查询结果缓存
 *   - 网页/图片资源缓存
 *   - API 响应缓存
 *   - 计算结果缓存（记忆化）
 */

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <list>
#include <functional>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;

// ======================== 辅助函数：获取当前时间字符串 ========================
string getCurrentTime() {
    auto now = chrono::system_clock::now();
    auto time_t_now = chrono::system_clock::to_time_t(now);
    auto ms = chrono::duration_cast<chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    stringstream ss;
    ss << put_time(localtime(&time_t_now), "%H:%M:%S");
    ss << "." << setfill('0') << setw(3) << ms.count();
    return ss.str();
}

// ======================== 缓存统计信息 ========================
struct CacheStats {
    int hits = 0;           // 命中次数
    int misses = 0;         // 未命中次数
    int totalGets = 0;      // 总获取次数
    int totalPuts = 0;      // 总写入次数
    int evictions = 0;      // 淘汰次数

    double hitRate() const {
        if (totalGets == 0) return 0.0;
        return static_cast<double>(hits) / totalGets * 100.0;
    }

    void print(const string& cacheName) const {
        cout << "    📊 " << cacheName << " 缓存统计:" << endl;
        cout << "       总获取: " << totalGets 
             << " | 命中: " << hits 
             << " | 未命中: " << misses << endl;
        cout << "       命中率: " << fixed << setprecision(1) << hitRate() << "%" 
             << " | 总写入: " << totalPuts
             << " | 淘汰: " << evictions << endl;
    }
};

// ======================== 缓存项（包含值和元数据） ========================
template<typename V>
struct CacheItem {
    V value;
    string createdAt;
    
    CacheItem(const V& v) : value(v), createdAt(getCurrentTime()) {}
};

// ======================== LRU缓存实现 ========================
// 原理：使用双向链表维护访问顺序，哈希表实现O(1)查找
// 当缓存满时，淘汰链表尾部（最久未访问）的元素
template<typename K, typename V>
class LRUCache {
public:
    explicit LRUCache(int capacity) : capacity_(capacity) {}

    // 获取缓存项
    bool get(const K& key, V& result) {
        stats_.totalGets++;
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end()) {
            stats_.misses++;
            cout << "    ❌ [LRU] 缓存未命中: " << key << endl;
            return false;
        }
        
        // 命中：将该项移到链表头部（最近使用）
        accessList_.splice(accessList_.begin(), accessList_, it->second);
        stats_.hits++;
        result = it->second->second.value;
        cout << "    ✅ [LRU] 缓存命中: " << key 
             << " -> " << result << endl;
        return true;
    }

    // 写入缓存项
    void put(const K& key, const V& value) {
        stats_.totalPuts++;
        auto it = cacheMap_.find(key);
        
        if (it != cacheMap_.end()) {
            // 已存在：更新值并移到链表头部
            it->second->second = CacheItem<V>(value);
            accessList_.splice(accessList_.begin(), accessList_, it->second);
            cout << "    🔄 [LRU] 更新缓存: " << key << " -> " << value << endl;
            return;
        }
        
        // 缓存已满：淘汰最久未访问的（链表尾部）
        if (static_cast<int>(cacheMap_.size()) >= capacity_) {
            auto& lastItem = accessList_.back();
            cout << "    🗑️  [LRU] 淘汰: " << lastItem.first 
                 << " -> " << lastItem.second.value << endl;
            cacheMap_.erase(lastItem.first);
            accessList_.pop_back();
            stats_.evictions++;
        }
        
        // 插入新项到链表头部
        accessList_.emplace_front(key, CacheItem<V>(value));
        cacheMap_[key] = accessList_.begin();
        cout << "    📝 [LRU] 写入缓存: " << key << " -> " << value << endl;
    }

    // 删除缓存项
    bool remove(const K& key) {
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end()) return false;
        
        accessList_.erase(it->second);
        cacheMap_.erase(it);
        cout << "    🗑️  [LRU] 删除缓存: " << key << endl;
        return true;
    }

    // 清空缓存
    void clear() {
        cacheMap_.clear();
        accessList_.clear();
        cout << "    🧹 [LRU] 缓存已清空" << endl;
    }

    // 当前缓存大小
    int size() const { return static_cast<int>(cacheMap_.size()); }

    // 获取统计信息
    const CacheStats& stats() const { return stats_; }

    // 打印缓存内容（按访问顺序）
    void printContents() const {
        cout << "    📋 [LRU] 缓存内容 (按访问顺序):" << endl;
        for (const auto& item : accessList_) {
            cout << "       " << item.first << " -> " << item.second.value 
                 << " (创建于 " << item.second.createdAt << ")" << endl;
        }
    }

private:
    int capacity_;
    // 双向链表：存储 <key, CacheItem>，头部是最近使用的
    using ListIter = typename list<pair<K, CacheItem<V>>>::iterator;
    list<pair<K, CacheItem<V>>> accessList_;
    unordered_map<K, ListIter> cacheMap_;
    CacheStats stats_;
};

// ======================== LFU缓存实现 ========================
// 原理：为每个频率维护一个链表，哈希表实现O(1)查找
// 当缓存满时，淘汰访问频率最低且最早加入的元素
template<typename K, typename V>
class LFUCache {
public:
    explicit LFUCache(int capacity) : capacity_(capacity), minFreq_(0) {}

    // 获取缓存项
    bool get(const K& key, V& result) {
        stats_.totalGets++;
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end()) {
            stats_.misses++;
            cout << "    ❌ [LFU] 缓存未命中: " << key << endl;
            return false;
        }
        
        stats_.hits++;
        result = it->second->second.value;
        // 增加频率
        increaseFrequency(key);
        cout << "    ✅ [LFU] 缓存命中: " << key 
             << " -> " << result 
             << " (频率: " << cacheMap_[key]->second.freq << ")" << endl;
        return true;
    }

    // 写入缓存项
    void put(const K& key, const V& value) {
        if (capacity_ <= 0) return;
        
        stats_.totalPuts++;
        auto it = cacheMap_.find(key);
        
        if (it != cacheMap_.end()) {
            // 已存在：更新值并增加频率
            it->second->second.value = value;
            increaseFrequency(key);
            cout << "    🔄 [LFU] 更新缓存: " << key << " -> " << value << endl;
            return;
        }
        
        // 缓存已满：淘汰频率最低且最早加入的元素
        if (static_cast<int>(cacheMap_.size()) >= capacity_) {
            auto& minFreqList = freqListMap_[minFreq_];
            auto& victim = minFreqList.back();
            cout << "    🗑️  [LFU] 淘汰: " << victim.first 
                 << " -> " << victim.second.value 
                 << " (频率: " << victim.second.freq << ")" << endl;
            cacheMap_.erase(victim.first);
            minFreqList.pop_back();
            if (minFreqList.empty()) {
                freqListMap_.erase(minFreq_);
            }
            stats_.evictions++;
        }
        
        // 插入新项（频率为1）
        minFreq_ = 1;
        freqListMap_[1].emplace_front(key, LFUNode(key, value));
        cacheMap_[key] = freqListMap_[1].begin();
        cout << "    📝 [LFU] 写入缓存: " << key << " -> " << value << endl;
    }

    // 删除缓存项
    bool remove(const K& key) {
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end()) return false;
        
        int freq = it->second->freq;
        freqListMap_[freq].erase(it->second);
        if (freqListMap_[freq].empty()) {
            freqListMap_.erase(freq);
            if (minFreq_ == freq) minFreq_++;
        }
        cacheMap_.erase(it);
        cout << "    🗑️  [LFU] 删除缓存: " << key << endl;
        return true;
    }

    // 清空缓存
    void clear() {
        cacheMap_.clear();
        freqListMap_.clear();
        minFreq_ = 0;
        cout << "    🧹 [LFU] 缓存已清空" << endl;
    }

    // 当前缓存大小
    int size() const { return static_cast<int>(cacheMap_.size()); }

    // 获取统计信息
    const CacheStats& stats() const { return stats_; }

    // 打印缓存内容
    void printContents() const {
        cout << "    📋 [LFU] 缓存内容 (按频率分组):" << endl;
        for (auto& [freq, freqList] : freqListMap_) {
            for (auto& item : freqList) {
                cout << "       " << item.first << " -> " << item.second.value 
                     << " (频率: " << freq << ")" << endl;
            }
        }
    }

private:
    struct LFUNode {
        K key;
        V value;
        int freq;
        LFUNode(const K& k, const V& v) : key(k), value(v), freq(1) {}
    };

    // 增加指定key的访问频率
    void increaseFrequency(const K& key) {
        auto it = cacheMap_[key];
        int oldFreq = it->second.freq;
        int newFreq = oldFreq + 1;
        
        // 保存节点数据
        K nodeKey = it->second.key;
        V nodeValue = it->second.value;
        
        // 从旧频率链表中移除
        freqListMap_[oldFreq].erase(it);
        if (freqListMap_[oldFreq].empty()) {
            freqListMap_.erase(oldFreq);
            if (minFreq_ == oldFreq) minFreq_ = newFreq;
        }
        
        // 插入新频率链表头部
        freqListMap_[newFreq].emplace_front(nodeKey, LFUNode(nodeKey, nodeValue));
        freqListMap_[newFreq].begin()->second.freq = newFreq;
        cacheMap_[key] = freqListMap_[newFreq].begin();
    }

    int capacity_;
    int minFreq_;
    
    // 频率 -> 该频率下的链表（存储key和LFUNode）
    using FreqList = list<pair<K, LFUNode>>;
    using FreqListIter = typename FreqList::iterator;
    
    unordered_map<int, FreqList> freqListMap_;
    unordered_map<K, FreqListIter> cacheMap_;
    CacheStats stats_;
};

// ======================== 辅助函数：打印分隔线 ========================
void printSeparator(const string& title) {
    cout << endl;
    cout << "════════════════════════════════════════" << endl;
    cout << "  " << title << endl;
    cout << "════════════════════════════════════════" << endl;
}

void printStep(int step, int total, const string& desc) {
    cout << endl;
    cout << "  [" << step << "/" << total << "] " << desc << endl;
}

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "     缓存模式演示 - LRU & LFU 缓存     " << endl;
    cout << "========================================" << endl;

    // ========== 场景一：LRU缓存演示 ==========
    printSeparator("场景一：LRU缓存 - 网页资源缓存");
    cout << endl;
    cout << "  模拟浏览器缓存网页资源，容量为4" << endl;
    cout << "  LRU策略：淘汰最长时间未被访问的资源" << endl;

    LRUCache<string, string> lruCache(4);
    string result;

    // 1. 写入缓存
    printStep(1, 6, "写入4个网页资源（缓存满）");
    lruCache.put("index.html", "<html>首页</html>");
    lruCache.put("style.css", "body { margin: 0; }");
    lruCache.put("app.js", "console.log('hello')");
    lruCache.put("logo.png", "[二进制图片数据 12KB]");
    cout << endl;
    lruCache.printContents();

    // 2. 访问已有资源（命中）
    printStep(2, 6, "访问已有资源（缓存命中）");
    lruCache.get("index.html", result);
    lruCache.get("style.css", result);
    cout << endl;
    cout << "  💡 访问后，index.html 和 style.css 被移到链表头部" << endl;
    lruCache.printContents();

    // 3. 写入新资源（触发淘汰）
    printStep(3, 6, "写入新资源（触发LRU淘汰）");
    cout << "  💡 app.js 是最久未访问的，将被淘汰" << endl;
    lruCache.put("api.json", "{ \"data\": [1,2,3] }");
    cout << endl;
    lruCache.printContents();

    // 4. 再次写入（淘汰logo.png）
    printStep(4, 6, "再次写入新资源");
    cout << "  💡 logo.png 成为最久未访问的，将被淘汰" << endl;
    lruCache.put("favicon.ico", "[图标数据 1KB]");
    cout << endl;
    lruCache.printContents();

    // 5. 更新已有缓存
    printStep(5, 6, "更新已有缓存项");
    lruCache.put("index.html", "<html>更新后的首页</html>");
    cout << endl;
    lruCache.printContents();

    // 6. 统计信息
    printStep(6, 6, "缓存统计");
    lruCache.stats().print("LRU");

    // ========== 场景二：LFU缓存演示 ==========
    printSeparator("场景二：LFU缓存 - 数据库查询缓存");
    cout << endl;
    cout << "  模拟数据库查询结果缓存，容量为4" << endl;
    cout << "  LFU策略：淘汰访问频率最低的查询结果" << endl;

    LFUCache<string, string> lfuCache(4);

    // 1. 写入缓存
    printStep(1, 7, "写入4个查询结果（缓存满）");
    lfuCache.put("SELECT * FROM users WHERE id=1", "{ name: '张三', age: 25 }");
    lfuCache.put("SELECT * FROM products", "[{id:1, name:'手机'}, {id:2, name:'电脑'}]");
    lfuCache.put("SELECT count(*) FROM orders", "15230");
    lfuCache.put("SELECT * FROM config", "{ theme: 'dark', lang: 'zh' }");
    cout << endl;
    lfuCache.printContents();

    // 2. 高频访问特定查询
    printStep(2, 7, "高频访问特定查询");
    cout << "  💡 多次查询用户信息和订单数量，提升它们的频率" << endl;
    lfuCache.get("SELECT * FROM users WHERE id=1", result);  // freq: 2
    lfuCache.get("SELECT * FROM users WHERE id=1", result);  // freq: 3
    lfuCache.get("SELECT count(*) FROM orders", result);     // freq: 2
    lfuCache.get("SELECT count(*) FROM orders", result);     // freq: 3
    lfuCache.get("SELECT count(*) FROM orders", result);     // freq: 4
    cout << endl;
    lfuCache.printContents();

    // 3. 写入新查询（触发淘汰）
    printStep(3, 7, "写入新查询结果（触发LFU淘汰）");
    cout << "  💡 products查询和config查询频率都是1（最低）" << endl;
    cout << "  💡 淘汰最早加入的那个（products）" << endl;
    lfuCache.put("SELECT * FROM categories", "[电子产品, 服装, 食品]");
    cout << endl;
    lfuCache.printContents();

    // 4. 再次写入
    printStep(4, 7, "再次写入新查询");
    cout << "  💡 config查询频率为1（最低），被淘汰" << endl;
    lfuCache.put("SELECT * FROM settings", "{ debug: true }");
    cout << endl;
    lfuCache.printContents();

    // 5. 访问低频项提升频率
    printStep(5, 7, "访问低频项提升频率");
    lfuCache.get("SELECT * FROM categories", result);  // freq: 2
    lfuCache.get("SELECT * FROM categories", result);  // freq: 3
    lfuCache.get("SELECT * FROM settings", result);    // freq: 2
    cout << endl;
    lfuCache.printContents();

    // 6. 再次淘汰
    printStep(6, 7, "写入新查询（再次触发淘汰）");
    cout << "  💡 users查询(freq=3)、orders查询(freq=4)、categories(freq=3)、settings(freq=2)" << endl;
    cout << "  💡 settings频率最低(2)，被淘汰" << endl;
    lfuCache.put("SELECT avg(price) FROM products", "2999.50");
    cout << endl;
    lfuCache.printContents();

    // 7. 统计信息
    printStep(7, 7, "缓存统计");
    lfuCache.stats().print("LFU");

    // ========== 场景三：LRU vs LFU 对比分析 ==========
    printSeparator("场景三：LRU vs LFU 对比分析");
    cout << endl;

    cout << "  ┌─────────────────┬─────────────────────┬─────────────────────┐" << endl;
    cout << "  │     特性         │       LRU           │       LFU           │" << endl;
    cout << "  ├─────────────────┼─────────────────────┼─────────────────────┤" << endl;
    cout << "  │ 淘汰策略         │ 最久未访问          │ 访问频率最低        │" << endl;
    cout << "  │ 数据结构         │ 双向链表 + 哈希表   │ 频率链表 + 哈希表   │" << endl;
    cout << "  │ 时间复杂度       │ O(1) get/put        │ O(1) get/put        │" << endl;
    cout << "  │ 空间复杂度       │ O(n)                │ O(n)                │" << endl;
    cout << "  │ 适用场景         │ 访问模式有时间局部性 │ 访问频率差异明显    │" << endl;
    cout << "  │ 抗突发流量       │ ⭐⭐⭐ 较好         │ ⭐⭐ 一般           │" << endl;
    cout << "  │ 长期热点保护     │ ⭐⭐ 一般           │ ⭐⭐⭐ 较好         │" << endl;
    cout << "  │ 实现复杂度       │ ⭐⭐ 简单           │ ⭐⭐⭐ 较复杂       │" << endl;
    cout << "  └─────────────────┴─────────────────────┴─────────────────────┘" << endl;

    // ========== 场景四：缓存穿透/击穿/雪崩防护 ==========
    printSeparator("场景四：缓存模式的常见问题与防护");
    cout << endl;

    cout << "  1. 缓存穿透 (Cache Penetration)" << endl;
    cout << "     问题：查询不存在的数据，每次都穿透缓存访问数据库" << endl;
    cout << "     防护：布隆过滤器 / 缓存空值" << endl;
    cout << endl;

    cout << "  2. 缓存击穿 (Cache Breakdown)" << endl;
    cout << "     问题：热点数据过期瞬间，大量请求直接打到数据库" << endl;
    cout << "     防护：互斥锁 / 永不过期 + 异步更新" << endl;
    cout << endl;

    cout << "  3. 缓存雪崩 (Cache Avalanche)" << endl;
    cout << "     问题：大量缓存同时过期，请求全部打到数据库" << endl;
    cout << "     防护：过期时间加随机偏移 / 多级缓存 / 熔断降级" << endl;
    cout << endl;

    cout << "  4. 缓存一致性 (Cache Consistency)" << endl;
    cout << "     问题：缓存与数据源数据不一致" << endl;
    cout << "     策略：Cache-Aside / Write-Through / Write-Behind / Read-Through" << endl;

    // ========== 优势总结 ==========
    cout << endl;
    cout << ">>> 缓存模式优势总结 <<<" << endl;
    cout << "  ├─ 性能提升：减少对低速数据源（数据库、网络）的访问" << endl;
    cout << "  │  （热点数据命中缓存后，响应时间可从毫秒级降到微秒级）" << endl;
    cout << "  ├─ 降低负载：减少后端服务和数据库的压力" << endl;
    cout << "  │  （大部分读请求被缓存拦截）" << endl;
    cout << "  ├─ 提高可用性：后端故障时缓存仍可提供服务" << endl;
    cout << "  │  （降级策略：返回缓存中的旧数据）" << endl;
    cout << "  ├─ 策略灵活：LRU/LFU/LRU-K等多种淘汰策略可选" << endl;
    cout << "  │  （根据业务特点选择最合适的策略）" << endl;
    cout << "  └─ 架构解耦：缓存层隔离了读写路径" << endl;
    cout << "     （读写可以独立优化和扩展）" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}