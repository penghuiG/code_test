/**
 * 模板方法模式 (Template Method Pattern)
 * 
 * 定义一个操作中算法的骨架，而将一些步骤延迟到子类中。
 * 模板方法使得子类可以在不改变算法结构的情况下，重新定义算法的某些特定步骤。
 * 
 * 本例以数据ETL（抽取-转换-加载）处理管线为场景，演示模板方法模式的核心结构：
 *   - AbstractClass（抽象类）：DataProcessor，定义数据处理的模板方法 process()
 *     模板方法依次调用：extract → validate → transform → load → notify
 *     同时提供钩子方法 hookPostProcess() 供子类选择性覆盖
 *   - ConcreteClass（具体类）：
 *     · CSVDataProcessor：从CSV文件抽取数据，按逗号分隔解析，加载到数据仓库
 *     · JSONDataProcessor：从JSON接口抽取数据，解析嵌套结构，加载到NoSQL数据库
 *     · DatabaseDataProcessor：从关系型数据库抽取数据，SQL清洗转换，加载到OLAP引擎
 *     · XMLDataProcessor：从XML文档抽取数据，XPath解析，加载到搜索引擎
 *   - TemplateMethod（模板方法）：process()，定义完整的处理流程骨架
 * 
 * 场景说明：
 *   一个数据分析平台需要从多种数据源（CSV文件、JSON接口、数据库、XML文档）
 *   抽取数据并进行清洗转换后加载到目标存储。每种数据源的抽取、校验、转换、
 *   加载逻辑各不相同，但处理流程的顺序和骨架是一致的。
 *   使用模板方法模式，将固定流程定义在基类中，将变化的步骤留给子类实现，
 *   既保证了流程的一致性，又允许各数据源灵活定制，符合好莱坞原则。
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <ctime>
using namespace std;

// ======================== 数据记录 ========================
struct DataRecord {
    string id;
    string source;
    string content;
    string timestamp;
};

// ======================== 处理结果 ========================
struct ProcessResult {
    bool   success;
    int    recordsProcessed;
    int    recordsFailed;
    string message;
    double durationMs;
};

// ======================== 抽象类：数据处理器 ========================
class DataProcessor {
public:
    virtual ~DataProcessor() {}

    // ===== 模板方法：定义处理流程骨架（不可被子类覆盖） =====
    ProcessResult process() {
        cout << "    ╔══════════════════════════════════════════════╗" << endl;
        cout << "    ║  [" << getProcessorName() << "] 开始数据处理管线  ║" << endl;
        cout << "    ╚══════════════════════════════════════════════╝" << endl;
        cout << endl;

        ProcessResult result{false, 0, 0, "", 0.0};

        // 步骤1：抽取数据
        cout << "    [步骤 1/5] 抽取数据 (Extract)" << endl;
        cout << "    ├─ 数据源: " << getDataSource() << endl;
        vector<DataRecord> rawData = extract();
        cout << "    └─ 抽取完成，共 " << rawData.size() << " 条原始记录" << endl;
        cout << endl;

        if (rawData.empty()) {
            result.message = "未抽取到数据";
            cout << "    [警告] 数据源为空，管线终止" << endl;
            return result;
        }

        // 步骤2：校验数据
        cout << "    [步骤 2/5] 校验数据 (Validate)" << endl;
        int failedCount = 0;
        vector<DataRecord> validData = validate(rawData, failedCount);
        result.recordsFailed = failedCount;
        cout << "    └─ 校验完成，有效: " << validData.size()
             << " 条，无效: " << failedCount << " 条" << endl;
        cout << endl;

        if (validData.empty()) {
            result.message = "所有数据校验失败";
            cout << "    [错误] 无有效数据，管线终止" << endl;
            return result;
        }

        // 步骤3：转换数据
        cout << "    [步骤 3/5] 转换数据 (Transform)" << endl;
        vector<DataRecord> transformedData = transform(validData);
        cout << "    └─ 转换完成，" << transformedData.size() << " 条记录就绪" << endl;
        cout << endl;

        // 步骤4：加载数据
        cout << "    [步骤 4/5] 加载数据 (Load)" << endl;
        bool loadSuccess = load(transformedData);
        result.recordsProcessed = static_cast<int>(transformedData.size());
        cout << "    └─ 加载" << (loadSuccess ? "成功" : "失败") << endl;
        cout << endl;

        // 步骤5：通知
        cout << "    [步骤 5/5] 发送通知 (Notify)" << endl;
        notify(result);
        cout << endl;

        // 钩子方法：后处理（子类可选择性覆盖）
        hookPostProcess(result);

        result.success = loadSuccess;
        result.message = loadSuccess ? "数据处理完成" : "数据加载失败";

        return result;
    }

protected:
    // ===== 抽象方法：由子类必须实现的步骤 =====
    virtual string getProcessorName() const = 0;
    virtual string getDataSource() const = 0;
    virtual vector<DataRecord> extract() = 0;
    virtual bool validateRecord(const DataRecord& record) const = 0;
    virtual DataRecord transformRecord(const DataRecord& record) const = 0;
    virtual bool load(const vector<DataRecord>& data) = 0;

    // ===== 可复用的默认实现 =====
    vector<DataRecord> validate(const vector<DataRecord>& rawData, int& failedCount) {
        vector<DataRecord> validData;
        failedCount = 0;
        for (const auto& record : rawData) {
            if (validateRecord(record)) {
                cout << "    ├─ ✓ 记录 [" << record.id << "] 校验通过" << endl;
                validData.push_back(record);
            } else {
                cout << "    ├─ ✗ 记录 [" << record.id << "] 校验失败，已丢弃" << endl;
                failedCount++;
            }
        }
        return validData;
    }

    vector<DataRecord> transform(const vector<DataRecord>& data) {
        vector<DataRecord> transformed;
        for (const auto& record : data) {
            DataRecord result = transformRecord(record);
            cout << "    ├─ 记录 [" << record.id << "] 转换完成" << endl;
            transformed.push_back(result);
        }
        return transformed;
    }

    void notify(const ProcessResult& result) {
        string status = result.success ? "成功" : "失败";
        cout << "    ├─ 发送邮件通知管理员: 数据处理" << status << endl;
        cout << "    ├─ 处理记录数: " << result.recordsProcessed << endl;
        cout << "    ├─ 失败记录数: " << result.recordsFailed << endl;
        cout << "    └─ 通知发送完成" << endl;
    }

    // ===== 钩子方法：默认空实现，子类可选择性覆盖 =====
    virtual void hookPostProcess(ProcessResult& result) {
        // 默认不做任何后处理
    }
};

// ======================== 具体类A：CSV数据处理器 ========================
class CSVDataProcessor : public DataProcessor {
public:
    CSVDataProcessor(const string& filePath) : filePath_(filePath) {}

protected:
    string getProcessorName() const override { return "CSV处理器"; }
    string getDataSource() const override { return "CSV文件: " + filePath_; }

    vector<DataRecord> extract() override {
        cout << "    ├─ 打开CSV文件: " << filePath_ << endl;
        cout << "    ├─ 解析CSV表头: id, name, email, amount" << endl;
        cout << "    ├─ 按行读取数据..." << endl;

        // 模拟从CSV文件读取数据
        return {
            {"CSV-001", "csv", "张三,zhangsan@mail.com,1500.00", "2026-07-28 09:00:00"},
            {"CSV-002", "csv", "李四,lisi@mail.com,2300.50",   "2026-07-28 09:01:00"},
            {"CSV-003", "csv", "王五,,800.00",                 "2026-07-28 09:02:00"},
            {"CSV-004", "csv", "赵六,zhaoliu@mail.com,3200.75","2026-07-28 09:03:00"},
            {"CSV-005", "csv", "孙七,sunqi@invalid,0",         "2026-07-28 09:04:00"},
        };
    }

    bool validateRecord(const DataRecord& record) const override {
        // CSV校验：检查是否有空字段、金额是否大于0
        string content = record.content;
        if (content.find(",,") != string::npos || content.empty()) return false;
        // 检查金额（最后一个字段）
        size_t lastComma = content.rfind(',');
        if (lastComma == string::npos) return false;
        string amountStr = content.substr(lastComma + 1);
        try {
            double amount = stod(amountStr);
            if (amount <= 0) return false;
        } catch (...) {
            return false;
        }
        return true;
    }

    DataRecord transformRecord(const DataRecord& record) const override {
        DataRecord result = record;
        // CSV转换：统一格式，添加来源标记
        result.content = "[CSV清洗] " + record.content;
        return result;
    }

    bool load(const vector<DataRecord>& data) override {
        cout << "    ├─ 连接数据仓库 (Data Warehouse)..." << endl;
        cout << "    ├─ 创建目标表: dw_csv_import" << endl;
        cout << "    ├─ 批量插入 " << data.size() << " 条记录..." << endl;
        for (const auto& record : data) {
            cout << "    │   INSERT INTO dw_csv_import VALUES('" << record.id << "', ...)" << endl;
        }
        cout << "    ├─ 提交事务" << endl;
        return true;
    }

    // 覆盖钩子方法：CSV处理后生成导入报告
    void hookPostProcess(ProcessResult& result) override {
        cout << "    [钩子] CSV后处理: 生成导入报告到 reports/csv_import_" << time(nullptr) << ".csv" << endl;
        cout << "    [钩子] 清理临时文件: /tmp/csv_buffer_" << time(nullptr) << ".tmp" << endl;
    }

private:
    string filePath_;
};

// ======================== 具体类B：JSON数据处理器 ========================
class JSONDataProcessor : public DataProcessor {
public:
    JSONDataProcessor(const string& apiUrl, const string& apiKey)
        : apiUrl_(apiUrl), apiKey_(apiKey) {}

protected:
    string getProcessorName() const override { return "JSON处理器"; }
    string getDataSource() const override { return "REST API: " + apiUrl_; }

    vector<DataRecord> extract() override {
        cout << "    ├─ 发起HTTP GET请求: " << apiUrl_ << endl;
        cout << "    ├─ 携带API Key认证..." << endl;
        cout << "    ├─ 接收JSON响应 (Content-Type: application/json)" << endl;
        cout << "    ├─ 解析JSON数组，共5个对象..." << endl;

        return {
            {"JSON-001", "json", R"({"user":"张三","score":95,"grade":"A"})", "2026-07-28 10:00:00"},
            {"JSON-002", "json", R"({"user":"李四","score":82,"grade":"B"})", "2026-07-28 10:01:00"},
            {"JSON-003", "json", R"({"user":"王五","score":-1,"grade":""})",  "2026-07-28 10:02:00"},
            {"JSON-004", "json", R"({"user":"赵六","score":78,"grade":"C"})", "2026-07-28 10:03:00"},
            {"JSON-005", "json", R"({"user":"","score":0,"grade":""})",       "2026-07-28 10:04:00"},
        };
    }

    bool validateRecord(const DataRecord& record) const override {
        // JSON校验：检查关键字段是否存在且有效
        string content = record.content;
        if (content.find("\"user\":\"\"") != string::npos) return false;
        if (content.find("\"score\":-1") != string::npos) return false;
        if (content.find("\"grade\":\"\"") != string::npos) return false;
        return true;
    }

    DataRecord transformRecord(const DataRecord& record) const override {
        DataRecord result = record;
        // JSON转换：标准化JSON格式，添加元数据
        result.content = "{\"source\":\"api\",\"data\":" + record.content + "}";
        return result;
    }

    bool load(const vector<DataRecord>& data) override {
        cout << "    ├─ 连接MongoDB集群..." << endl;
        cout << "    ├─ 选择数据库: analytics_db" << endl;
        cout << "    ├─ 选择集合: api_imports" << endl;
        cout << "    ├─ 批量写入 " << data.size() << " 个文档..." << endl;
        for (const auto& record : data) {
            cout << "    │   db.api_imports.insertOne({_id:\"" << record.id << "\", ...})" << endl;
        }
        return true;
    }

    // 覆盖钩子方法：JSON处理后更新API调用统计
    void hookPostProcess(ProcessResult& result) override {
        cout << "    [钩子] JSON后处理: 更新API调用计数器 (+1)" << endl;
        cout << "    [钩子] 缓存最新抽取时间戳到Redis" << endl;
    }

private:
    string apiUrl_;
    string apiKey_;
};

// ======================== 具体类C：数据库数据处理器 ========================
class DatabaseDataProcessor : public DataProcessor {
public:
    DatabaseDataProcessor(const string& connStr, const string& query)
        : connStr_(connStr), query_(query) {}

protected:
    string getProcessorName() const override { return "数据库处理器"; }
    string getDataSource() const override { return "MySQL: " + connStr_ + " | SQL: " + query_; }

    vector<DataRecord> extract() override {
        cout << "    ├─ 建立MySQL连接: " << connStr_ << endl;
        cout << "    ├─ 执行查询: " << query_ << endl;
        cout << "    ├─ 获取结果集，游标遍历..." << endl;

        return {
            {"DB-001", "mysql", "order_id=1001,amount=299.00,status=paid",    "2026-07-28 08:00:00"},
            {"DB-002", "mysql", "order_id=1002,amount=599.00,status=paid",    "2026-07-28 08:01:00"},
            {"DB-003", "mysql", "order_id=1003,amount=0.00,status=cancelled", "2026-07-28 08:02:00"},
            {"DB-004", "mysql", "order_id=1004,amount=1299.00,status=paid",   "2026-07-28 08:03:00"},
            {"DB-005", "mysql", "order_id=1005,amount=459.00,status=refunded","2026-07-28 08:04:00"},
            {"DB-006", "mysql", "order_id=1006,amount=888.00,status=paid",    "2026-07-28 08:05:00"},
        };
    }

    bool validateRecord(const DataRecord& record) const override {
        // 数据库记录校验：跳过已取消和已退款的订单
        string content = record.content;
        if (content.find("status=cancelled") != string::npos) return false;
        if (content.find("status=refunded") != string::npos) return false;
        if (content.find("amount=0.00") != string::npos) return false;
        return true;
    }

    DataRecord transformRecord(const DataRecord& record) const override {
        DataRecord result = record;
        // 数据库转换：SQL风格清洗
        result.content = "SELECT * FROM enriched WHERE raw='" + record.content + "'";
        return result;
    }

    bool load(const vector<DataRecord>& data) override {
        cout << "    ├─ 连接ClickHouse OLAP引擎..." << endl;
        cout << "    ├─ 创建目标表: olap_order_analysis" << endl;
        cout << "    ├─ 批量INSERT INTO..." << endl;
        for (const auto& record : data) {
            cout << "    │   INSERT INTO olap_order_analysis('" << record.id << "', ...)" << endl;
        }
        cout << "    ├─ OPTIMIZE TABLE olap_order_analysis" << endl;
        return true;
    }

    // 覆盖钩子方法：数据库处理后刷新物化视图
    void hookPostProcess(ProcessResult& result) override {
        cout << "    [钩子] DB后处理: 刷新物化视图 mv_daily_summary" << endl;
        cout << "    [钩子] 更新ETL元数据表 etl_metadata" << endl;
        cout << "    [钩子] 释放数据库连接回连接池" << endl;
    }

private:
    string connStr_;
    string query_;
};

// ======================== 具体类D：XML数据处理器 ========================
class XMLDataProcessor : public DataProcessor {
public:
    XMLDataProcessor(const string& xmlPath, const string& schemaUrl)
        : xmlPath_(xmlPath), schemaUrl_(schemaUrl) {}

protected:
    string getProcessorName() const override { return "XML处理器"; }
    string getDataSource() const override { return "XML文件: " + xmlPath_ + " (Schema: " + schemaUrl_ + ")"; }

    vector<DataRecord> extract() override {
        cout << "    ├─ 读取XML文件: " << xmlPath_ << endl;
        cout << "    ├─ 加载XSD Schema进行预校验..." << endl;
        cout << "    ├─ 使用SAX解析器遍历XML节点..." << endl;
        cout << "    ├─ XPath查询: //catalog/item" << endl;

        return {
            {"XML-001", "xml", "<item><title>C++ Primer</title><price>128.0</price></item>",          "2026-07-28 07:00:00"},
            {"XML-002", "xml", "<item><title>Design Patterns</title><price>89.0</price></item>",      "2026-07-28 07:01:00"},
            {"XML-003", "xml", "<item><title></title><price>0</price></item>",                        "2026-07-28 07:02:00"},
            {"XML-004", "xml", "<item><title>Effective Modern C++</title><price>105.0</price></item>","2026-07-28 07:03:00"},
        };
    }

    bool validateRecord(const DataRecord& record) const override {
        // XML校验：检查节点是否为空、价格是否有效
        string content = record.content;
        if (content.find("<title></title>") != string::npos) return false;
        if (content.find("<price>0</price>") != string::npos) return false;
        return true;
    }

    DataRecord transformRecord(const DataRecord& record) const override {
        DataRecord result = record;
        // XML转换：添加CDATA包装，标准化节点
        result.content = "<enriched_item><![CDATA[" + record.content + "]]></enriched_item>";
        return result;
    }

    bool load(const vector<DataRecord>& data) override {
        cout << "    ├─ 连接Elasticsearch集群..." << endl;
        cout << "    ├─ 创建索引: xml_catalog_import" << endl;
        cout << "    ├─ 构建Bulk API请求..." << endl;
        for (const auto& record : data) {
            cout << "    │   {_index:\"xml_catalog\",_id:\"" << record.id << "\", ...}" << endl;
        }
        cout << "    ├─ 执行Bulk写入..." << endl;
        return true;
    }

    // 覆盖钩子方法：XML处理后触发搜索索引刷新
    void hookPostProcess(ProcessResult& result) override {
        cout << "    [钩子] XML后处理: POST /xml_catalog_import/_refresh" << endl;
        cout << "    [钩子] 更新分面搜索聚合缓存" << endl;
    }

private:
    string xmlPath_;
    string schemaUrl_;
};

// ======================== 辅助函数 ========================
void printSection(const string& title) {
    cout << endl;
    cout << "  ┌─ " << title << " ─────────────────────────────" << endl;
}

void printSectionEnd() {
    cout << "  └──────────────────────────────────────────────" << endl;
}

void printResult(const ProcessResult& result) {
    cout << "    ┌─────────────────────────────────────────┐" << endl;
    cout << "    │ 处理结果: " << (result.success ? "成功 ✓" : "失败 ✗") << endl;
    cout << "    │ 处理记录: " << result.recordsProcessed << " 条" << endl;
    cout << "    │ 失败记录: " << result.recordsFailed << " 条" << endl;
    cout << "    │ 说    明: " << result.message << endl;
    cout << "    └─────────────────────────────────────────┘" << endl;
}

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "    模板方法模式演示 - 数据ETL系统       " << endl;
    cout << "========================================" << endl;

    // ---------- 1. CSV数据处理 ----------
    cout << endl;
    cout << ">>> 场景一：从CSV文件导入数据 <<<" << endl;

    printSection("初始化CSV处理器");
    auto csvProcessor = make_shared<CSVDataProcessor>("/data/sales_20260728.csv");
    printSectionEnd();

    printSection("执行处理管线");
    ProcessResult csvResult = csvProcessor->process();
    printSectionEnd();

    printSection("结果汇总");
    printResult(csvResult);
    printSectionEnd();

    // ---------- 2. JSON API数据处理 ----------
    cout << endl;
    cout << ">>> 场景二：从JSON API抽取数据 <<<" << endl;

    printSection("初始化JSON处理器");
    auto jsonProcessor = make_shared<JSONDataProcessor>(
        "https://api.example.com/v2/scores", "sk-abc123xyz");
    printSectionEnd();

    printSection("执行处理管线");
    ProcessResult jsonResult = jsonProcessor->process();
    printSectionEnd();

    printSection("结果汇总");
    printResult(jsonResult);
    printSectionEnd();

    // ---------- 3. 数据库数据处理 ----------
    cout << endl;
    cout << ">>> 场景三：从MySQL数据库抽取数据 <<<" << endl;

    printSection("初始化数据库处理器");
    auto dbProcessor = make_shared<DatabaseDataProcessor>(
        "mysql://192.168.1.100:3306/production",
        "SELECT * FROM orders WHERE date >= '2026-07-28'");
    printSectionEnd();

    printSection("执行处理管线");
    ProcessResult dbResult = dbProcessor->process();
    printSectionEnd();

    printSection("结果汇总");
    printResult(dbResult);
    printSectionEnd();

    // ---------- 4. XML数据处理 ----------
    cout << endl;
    cout << ">>> 场景四：从XML文档导入数据 <<<" << endl;

    printSection("初始化XML处理器");
    auto xmlProcessor = make_shared<XMLDataProcessor>(
        "/data/catalog/books.xml",
        "http://example.com/schemas/catalog.xsd");
    printSectionEnd();

    printSection("执行处理管线");
    ProcessResult xmlResult = xmlProcessor->process();
    printSectionEnd();

    printSection("结果汇总");
    printResult(xmlResult);
    printSectionEnd();

    // ---------- 5. 多态批量处理 ----------
    cout << endl;
    cout << ">>> 场景五：多态批量处理 - 统一接口处理不同数据源 <<<" << endl;

    printSection("批量初始化处理器");
    vector<shared_ptr<DataProcessor>> processors;
    processors.push_back(make_shared<CSVDataProcessor>("/data/batch1.csv"));
    processors.push_back(make_shared<JSONDataProcessor>("https://api.example.com/batch", "key123"));
    processors.push_back(make_shared<DatabaseDataProcessor>("mysql://localhost/db", "SELECT * FROM t"));
    cout << "    共初始化 " << processors.size() << " 个处理器" << endl;
    printSectionEnd();

    printSection("批量执行");
    int successCount = 0;
    for (size_t i = 0; i < processors.size(); i++) {
        cout << endl;
        cout << "    ========== 批量任务 " << (i + 1) << "/" << processors.size() << " ==========" << endl;
        ProcessResult result = processors[i]->process();
        if (result.success) successCount++;
    }
    cout << endl;
    cout << "    批量处理完成: " << successCount << "/" << processors.size() << " 成功" << endl;
    printSectionEnd();

    // ---------- 6. 展示模板方法模式的价值 ----------
    cout << endl;
    cout << ">>> 模板方法模式优势总结 <<<" << endl;
    cout << "  ├─ 流程固定：基类定义算法骨架，保证处理流程一致性" << endl;
    cout << "  ├─ 步骤可变：子类只需实现差异化的抽象方法" << endl;
    cout << "  ├─ 复用代码：validate()、transform() 等复用逻辑在基类中实现" << endl;
    cout << "  ├─ 钩子扩展：hookPostProcess() 提供可选的扩展点" << endl;
    cout << "  ├─ 好莱坞原则：基类控制流程(\"别调用我们，我们会调用你\")" << endl;
    cout << "  ├─ 开闭原则：新增数据源只需添加新子类" << endl;
    cout << "  └─ 多态调用：客户端可使用统一接口处理不同数据源" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}