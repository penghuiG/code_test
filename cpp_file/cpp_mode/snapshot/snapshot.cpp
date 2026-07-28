/**
 * 备忘录模式 (Memento Pattern)
 * 
 * 在不破坏封装性的前提下，捕获一个对象的内部状态，并在该对象之外保存这个状态，
 * 以便在需要时能将该对象恢复到原先保存的状态。
 * 
 * 本例以文本编辑器为场景，演示备忘录模式的核心结构及多级撤销/重做功能：
 *   - Memento（备忘录）：EditorMemento，存储编辑器在某一时刻的状态快照
 *   - Originator（原发器）：TextEditor，拥有内部状态，可以创建和恢复备忘录
 *   - Caretaker（管理者）：HistoryManager，负责保存和管理备忘录，但不操作其内容
 * 
 * 场景说明：
 *   一个文本编辑器支持多级撤销（Undo）和重做（Redo）功能。用户每进行一次
 *   编辑操作，编辑器都会将当前状态保存为快照。当用户执行撤销操作时，编辑器
 *   从历史记录中恢复到之前的状态。当用户执行重做操作时，编辑器恢复到撤销前
 *   的状态。整个过程中，管理者只负责保存快照，不关心快照的具体内容。
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ctime>
using namespace std;

// ======================== 备忘录：编辑器状态快照 ========================
class EditorMemento {
public:
    EditorMemento(const string& content, int cursorPos, const string& timestamp)
        : content_(content), cursorPos_(cursorPos), timestamp_(timestamp) {}

    string getContent() const { return content_; }
    int getCursorPos() const { return cursorPos_; }
    string getTimestamp() const { return timestamp_; }

private:
    string content_;       // 文本内容
    int cursorPos_;        // 光标位置
    string timestamp_;     // 创建时间
};

// ======================== 原发器：文本编辑器 ========================
class TextEditor {
public:
    TextEditor() : content_(""), cursorPos_(0) {}

    // 设置文本内容
    void setContent(const string& content) {
        content_ = content;
        cout << "    [编辑器] 设置内容: \"" << content_ << "\"" << endl;
    }

    // 插入文本（在光标位置）
    void insertText(const string& text) {
        content_ = content_.substr(0, cursorPos_) + text + content_.substr(cursorPos_);
        cursorPos_ += text.size();
        cout << "    [编辑器] 插入文本: \"" << text << "\" → 当前内容: \"" << content_ << "\"" << endl;
    }

    // 删除光标前的字符
    void deleteBefore(int count) {
        if (cursorPos_ >= count) {
            string deleted = content_.substr(cursorPos_ - count, count);
            content_ = content_.substr(0, cursorPos_ - count) + content_.substr(cursorPos_);
            cursorPos_ -= count;
            cout << "    [编辑器] 删除 \"" << deleted << "\" → 当前内容: \"" << content_ << "\"" << endl;
        } else {
            cout << "    [编辑器] 删除失败：光标前不足 " << count << " 个字符" << endl;
        }
    }

    // 移动光标
    void moveCursor(int pos) {
        if (pos >= 0 && pos <= static_cast<int>(content_.size())) {
            cursorPos_ = pos;
            cout << "    [编辑器] 光标移动到位置 " << cursorPos_ << endl;
        } else {
            cout << "    [编辑器] 无效的光标位置: " << pos << endl;
        }
    }

    // 创建备忘录（保存当前状态）
    shared_ptr<EditorMemento> createMemento() const {
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        char buf[20];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);

        cout << "    [编辑器] 创建快照 → 内容: \"" << content_
             << "\", 光标: " << cursorPos_ << ", 时间: " << buf << endl;
        return make_shared<EditorMemento>(content_, cursorPos_, string(buf));
    }

    // 从备忘录恢复状态
    void restoreMemento(const shared_ptr<EditorMemento>& memento) {
        content_ = memento->getContent();
        cursorPos_ = memento->getCursorPos();
        cout << "    [编辑器] 恢复快照 (时间: " << memento->getTimestamp()
             << ") → 内容: \"" << content_ << "\", 光标: " << cursorPos_ << endl;
    }

    // 显示当前状态
    void showState() const {
        cout << "    ┌─────────────────────────────────" << endl;
        cout << "    │ 内容: \"" << content_ << "\"" << endl;
        cout << "    │ 光标: " << cursorPos_ << endl;
        cout << "    │ ";
        // 用 ^ 标记光标位置
        for (int i = 0; i < cursorPos_; i++) cout << " ";
        cout << "^" << endl;
        cout << "    └─────────────────────────────────" << endl;
    }

private:
    string content_;    // 文本内容
    int cursorPos_;     // 光标位置
};

// ======================== 管理者：历史记录管理器 ========================
class HistoryManager {
public:
    HistoryManager() : currentIdx_(-1) {}

    // 保存快照到历史记录
    void push(const shared_ptr<EditorMemento>& memento) {
        // 如果当前不在最新位置，丢弃后面的记录（分支覆盖）
        if (currentIdx_ < static_cast<int>(history_.size()) - 1) {
            int discardCount = history_.size() - 1 - currentIdx_;
            history_.erase(history_.begin() + currentIdx_ + 1, history_.end());
            cout << "    [历史管理器] 丢弃 " << discardCount << " 条后续记录（新的编辑覆盖了重做历史）" << endl;
        }
        history_.push_back(memento);
        currentIdx_ = history_.size() - 1;
        cout << "    [历史管理器] 保存快照 #" << currentIdx_
             << " (时间: " << memento->getTimestamp() << ")" << endl;
    }

    // 撤销：获取上一个快照
    shared_ptr<EditorMemento> undo() {
        if (currentIdx_ > 0) {
            currentIdx_--;
            cout << "    [历史管理器] 撤销到快照 #" << currentIdx_
                 << " (时间: " << history_[currentIdx_]->getTimestamp() << ")" << endl;
            return history_[currentIdx_];
        }
        cout << "    [历史管理器] 已经是最早的状态，无法撤销" << endl;
        return nullptr;
    }

    // 重做：获取下一个快照
    shared_ptr<EditorMemento> redo() {
        if (currentIdx_ < static_cast<int>(history_.size()) - 1) {
            currentIdx_++;
            cout << "    [历史管理器] 重做到快照 #" << currentIdx_
                 << " (时间: " << history_[currentIdx_]->getTimestamp() << ")" << endl;
            return history_[currentIdx_];
        }
        cout << "    [历史管理器] 已经是最新的状态，无法重做" << endl;
        return nullptr;
    }

    // 显示所有历史记录
    void showHistory() const {
        cout << "    ── 历史快照记录 ──────────────────────────" << endl;
        for (int i = 0; i < static_cast<int>(history_.size()); i++) {
            string marker = (i == currentIdx_) ? " ← [当前]" : "";
            cout << "    │ 快照 #" << i
                 << " | 时间: " << history_[i]->getTimestamp()
                 << " | 内容: \"" << history_[i]->getContent()
                 << "\" | 光标: " << history_[i]->getCursorPos() << marker << endl;
        }
        cout << "    ──────────────────────────────────────────" << endl;
    }

    int getCurrentIndex() const { return currentIdx_; }
    int getSize() const { return history_.size(); }

private:
    vector<shared_ptr<EditorMemento>> history_;   // 历史快照列表
    int currentIdx_;                               // 当前快照索引
};

// ======================== 辅助函数：分隔线 ========================
void printSection(const string& title) {
    cout << endl;
    cout << "  ┌─ " << title << " ─────────────────────────────" << endl;
}

void printSectionEnd() {
    cout << "  └──────────────────────────────────────────────" << endl;
}

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "       备忘录模式演示 - 文本编辑器        " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 创建编辑器和历史管理器 ----------
    cout << endl;
    cout << ">>> 步骤一：初始化编辑器和历史管理器 <<<" << endl;

    TextEditor editor;
    HistoryManager history;

    editor.showState();

    // ---------- 2. 初始编辑并保存快照 ----------
    cout << endl;
    cout << ">>> 步骤二：编辑文本并保存初始快照 <<<" << endl;

    printSection("输入文本");
    editor.insertText("Hello");
    // 自动保存快照
    history.push(editor.createMemento());
    printSectionEnd();

    // ---------- 3. 继续编辑 ----------
    printSection("继续输入");
    editor.insertText(" World");
    // 自动保存快照
    history.push(editor.createMemento());
    printSectionEnd();

    // ---------- 4. 再次编辑 ----------
    printSection("再插入文本");
    editor.insertText("!");
    // 自动保存快照
    history.push(editor.createMemento());
    printSectionEnd();

    // ---------- 5. 显示当前状态和历史记录 ----------
    cout << endl;
    cout << ">>> 步骤三：查看当前状态和历史记录 <<<" << endl;

    printSection("当前编辑器状态");
    editor.showState();
    printSectionEnd();

    printSection("历史快照记录");
    history.showHistory();
    printSectionEnd();

    // ---------- 6. 撤销操作 ----------
    cout << endl;
    cout << ">>> 步骤四：执行撤销操作（Undo） <<<" << endl;

    printSection("第一次撤销");
    auto m1 = history.undo();
    if (m1) editor.restoreMemento(m1);
    editor.showState();
    printSectionEnd();

    printSection("第二次撤销");
    auto m2 = history.undo();
    if (m2) editor.restoreMemento(m2);
    editor.showState();
    printSectionEnd();

    // ---------- 7. 重做操作 ----------
    cout << endl;
    cout << ">>> 步骤五：执行重做操作（Redo） <<<" << endl;

    printSection("第一次重做");
    auto r1 = history.redo();
    if (r1) editor.restoreMemento(r1);
    editor.showState();
    printSectionEnd();

    printSection("第二次重做");
    auto r2 = history.redo();
    if (r2) editor.restoreMemento(r2);
    editor.showState();
    printSectionEnd();

    // ---------- 8. 撤销后编辑（覆盖重做历史） ----------
    cout << endl;
    cout << ">>> 步骤六：撤销后编辑（覆盖重做历史） <<<" << endl;

    printSection("先撤销两次");
    history.undo();
    editor.restoreMemento(history.undo());
    editor.showState();
    printSectionEnd();

    printSection("执行新的编辑（将覆盖后续历史）");
    editor.insertText("C++ World");
    history.push(editor.createMemento());
    editor.showState();
    printSectionEnd();

    // ---------- 9. 最终状态 ----------
    cout << endl;
    cout << ">>> 步骤七：查看最终历史记录 <<<" << endl;

    printSection("最终历史快照记录");
    history.showHistory();
    printSectionEnd();

    printSection("尝试重做（已被覆盖）");
    auto failRedo = history.redo();
    if (!failRedo) {
        cout << "    [提示] 重做失败 —— 后续历史已被新编辑覆盖" << endl;
    }
    printSectionEnd();

    // ---------- 10. 展示备忘录模式的价值 ----------
    cout << endl;
    cout << ">>> 备忘录模式优势总结 <<<" << endl;
    cout << "  ├─ 封装性：备忘录对外隐藏了编辑器的内部状态细节" << endl;
    cout << "  ├─ 单一职责：编辑器负责编辑，管理者负责存储备忘录" << endl;
    cout << "  ├─ 撤销/重做：通过保存和恢复快照实现完整的撤销/重做功能" << endl;
    cout << "  ├─ 分支处理：新编辑自动覆盖后续重做历史，保证一致性" << endl;
    cout << "  ├─ 开闭原则：可以轻松扩展更多状态字段而不影响管理器" << endl;
    cout << "  └─ 透明性：客户端只需调用 createMemento/restoreMemento 接口" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}