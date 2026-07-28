/**
 * 访问者模式 (Visitor Pattern)
 * 
 * 表示一个作用于某对象结构中的各元素的操作。
 * 它使你可以在不改变各元素的类的前提下定义作用于这些元素的新操作。
 * 
 * 本例以文档处理系统为场景，演示访问者模式的核心结构：
 *   - Element（抽象元素）：DocumentElement，定义 accept(Visitor*) 接口
 *   - ConcreteElement（具体元素）：
 *     · TextElement：文本段落
 *     · ImageElement：图片
 *     · TableElement：表格
 *     · CodeBlockElement：代码块
 *     · LinkElement：超链接
 *   - Visitor（抽象访问者）：DocumentVisitor，为每种元素声明 visit() 方法
 *   - ConcreteVisitor（具体访问者）：
 *     · ExportHTMLVisitor：将文档导出为HTML
 *     · WordCountVisitor：统计文档字数
 *     · SpellCheckVisitor：拼写检查
 *     · ExtractImageVisitor：提取所有图片资源
 *     · TOCVisitor：生成目录结构
 * 
 * 场景说明：
 *   一个文档编辑器需要对文档树中的各种元素执行多种操作（导出HTML、字数统计、
 *   拼写检查、提取图片、生成目录等）。如果在每种元素类中都添加这些操作，
 *   会导致元素类不断膨胀，且每次新增操作都要修改所有元素类。
 *   使用访问者模式，将操作封装到独立的访问者中，新增操作只需添加新的访问者，
 *   无需修改元素类，符合开闭原则。元素通过 accept() 方法实现双重分派。
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <map>
using namespace std;

// ======================== 前置声明 ========================
class DocumentVisitor;
class TextElement;
class ImageElement;
class TableElement;
class CodeBlockElement;
class LinkElement;

// ======================== 抽象元素：文档元素 ========================
class DocumentElement {
public:
    virtual ~DocumentElement() {}
    virtual void accept(DocumentVisitor* visitor) = 0;
    virtual string getType() const = 0;
};

// ======================== 抽象访问者 ========================
class DocumentVisitor {
public:
    virtual ~DocumentVisitor() {}
    virtual void visit(TextElement* element) = 0;
    virtual void visit(ImageElement* element) = 0;
    virtual void visit(TableElement* element) = 0;
    virtual void visit(CodeBlockElement* element) = 0;
    virtual void visit(LinkElement* element) = 0;
    virtual string getVisitorName() const = 0;
};

// ======================== 具体元素A：文本段落 ========================
class TextElement : public DocumentElement {
public:
    TextElement(const string& content, const string& style = "normal")
        : content_(content), style_(style) {}

    void accept(DocumentVisitor* visitor) override {
        visitor->visit(this);
    }

    string getType() const override { return "文本段落"; }
    const string& getContent() const { return content_; }
    const string& getStyle() const { return style_; }

private:
    string content_;
    string style_;  // normal, heading1, heading2, bold, italic
};

// ======================== 具体元素B：图片 ========================
class ImageElement : public DocumentElement {
public:
    ImageElement(const string& path, int width, int height, const string& caption = "")
        : path_(path), width_(width), height_(height), caption_(caption) {}

    void accept(DocumentVisitor* visitor) override {
        visitor->visit(this);
    }

    string getType() const override { return "图片"; }
    const string& getPath() const { return path_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    const string& getCaption() const { return caption_; }

private:
    string path_;
    int width_;
    int height_;
    string caption_;
};

// ======================== 具体元素C：表格 ========================
struct TableCell {
    string content;
    bool isHeader;
};

class TableElement : public DocumentElement {
public:
    TableElement(const vector<string>& headers, const vector<vector<string>>& rows)
        : headers_(headers), rows_(rows) {}

    void accept(DocumentVisitor* visitor) override {
        visitor->visit(this);
    }

    string getType() const override { return "表格"; }
    const vector<string>& getHeaders() const { return headers_; }
    const vector<vector<string>>& getRows() const { return rows_; }
    int getRowCount() const { return static_cast<int>(rows_.size()); }
    int getColCount() const { return static_cast<int>(headers_.size()); }

private:
    vector<string> headers_;
    vector<vector<string>> rows_;
};

// ======================== 具体元素D：代码块 ========================
class CodeBlockElement : public DocumentElement {
public:
    CodeBlockElement(const string& code, const string& language)
        : code_(code), language_(language) {}

    void accept(DocumentVisitor* visitor) override {
        visitor->visit(this);
    }

    string getType() const override { return "代码块"; }
    const string& getCode() const { return code_; }
    const string& getLanguage() const { return language_; }

private:
    string code_;
    string language_;
};

// ======================== 具体元素E：超链接 ========================
class LinkElement : public DocumentElement {
public:
    LinkElement(const string& text, const string& url)
        : text_(text), url_(url) {}

    void accept(DocumentVisitor* visitor) override {
        visitor->visit(this);
    }

    string getType() const override { return "超链接"; }
    const string& getText() const { return text_; }
    const string& getUrl() const { return url_; }

private:
    string text_;
    string url_;
};

// ======================== 文档结构（对象结构） ========================
class Document {
public:
    Document(const string& title) : title_(title) {}

    void addElement(shared_ptr<DocumentElement> element) {
        elements_.push_back(element);
    }

    void accept(DocumentVisitor* visitor) {
        cout << "    [" << visitor->getVisitorName() << "] 开始处理文档: " << title_ << endl;
        for (auto& element : elements_) {
            element->accept(visitor);
        }
        cout << "    [" << visitor->getVisitorName() << "] 文档处理完成" << endl;
    }

    const string& getTitle() const { return title_; }
    const vector<shared_ptr<DocumentElement>>& getElements() const { return elements_; }

private:
    string title_;
    vector<shared_ptr<DocumentElement>> elements_;
};

// ======================== 具体访问者A：HTML导出访问者 ========================
class ExportHTMLVisitor : public DocumentVisitor {
public:
    void visit(TextElement* element) override {
        string tag = "p";
        if (element->getStyle() == "heading1") tag = "h1";
        else if (element->getStyle() == "heading2") tag = "h2";
        else if (element->getStyle() == "bold") tag = "strong";
        else if (element->getStyle() == "italic") tag = "em";

        htmlContent_ += "  <" + tag + ">" + element->getContent() + "</" + tag + ">\n";
        cout << "    ├─ 文本 → <" << tag << "> 标签" << endl;
    }

    void visit(ImageElement* element) override {
        htmlContent_ += "  <img src=\"" + element->getPath() + "\" "
                      + "width=\"" + to_string(element->getWidth()) + "\" "
                      + "height=\"" + to_string(element->getHeight()) + "\" "
                      + "alt=\"" + element->getCaption() + "\" />\n";
        if (!element->getCaption().empty()) {
            htmlContent_ += "  <p class=\"caption\">" + element->getCaption() + "</p>\n";
        }
        cout << "    ├─ 图片 → <img> 标签" << endl;
    }

    void visit(TableElement* element) override {
        htmlContent_ += "  <table border=\"1\">\n    <thead><tr>\n";
        for (const auto& header : element->getHeaders()) {
            htmlContent_ += "      <th>" + header + "</th>\n";
        }
        htmlContent_ += "    </tr></thead>\n    <tbody>\n";
        for (const auto& row : element->getRows()) {
            htmlContent_ += "    <tr>\n";
            for (const auto& cell : row) {
                htmlContent_ += "      <td>" + cell + "</td>\n";
            }
            htmlContent_ += "    </tr>\n";
        }
        htmlContent_ += "    </tbody>\n  </table>\n";
        cout << "    ├─ 表格 → <table> 标签 (" << element->getRowCount() << "行)" << endl;
    }

    void visit(CodeBlockElement* element) override {
        htmlContent_ += "  <pre><code class=\"language-" + element->getLanguage() + "\">"
                      + element->getCode() + "</code></pre>\n";
        cout << "    ├─ 代码 → <pre><code> 标签" << endl;
    }

    void visit(LinkElement* element) override {
        htmlContent_ += "  <a href=\"" + element->getUrl() + "\">"
                      + element->getText() + "</a>\n";
        cout << "    ├─ 链接 → <a> 标签" << endl;
    }

    string getVisitorName() const override { return "HTML导出访问者"; }

    string getHTML() const {
        return "<html>\n<head><title>Export</title></head>\n<body>\n"
             + htmlContent_ + "</body>\n</html>";
    }

private:
    string htmlContent_;
};

// ======================== 具体访问者B：字数统计访问者 ========================
class WordCountVisitor : public DocumentVisitor {
public:
    void visit(TextElement* element) override {
        int count = countWords(element->getContent());
        wordCount_ += count;
        charCount_ += static_cast<int>(element->getContent().size());
        elementCounts_["文本段落"]++;
        cout << "    ├─ 文本 \"" << truncate(element->getContent(), 20) << "\" → "
             << count << " 字, " << element->getContent().size() << " 字符" << endl;
    }

    void visit(ImageElement* element) override {
        imageCount_++;
        elementCounts_["图片"]++;
        cout << "    ├─ 图片 [" << element->getPath() << "] → 不计入字数" << endl;
    }

    void visit(TableElement* element) override {
        int cellWords = 0;
        for (const auto& header : element->getHeaders()) {
            cellWords += countWords(header);
        }
        for (const auto& row : element->getRows()) {
            for (const auto& cell : row) {
                cellWords += countWords(cell);
            }
        }
        wordCount_ += cellWords;
        tableCount_++;
        elementCounts_["表格"]++;
        cout << "    ├─ 表格 (" << element->getRowCount() << "×"
             << element->getColCount() << ") → " << cellWords << " 字" << endl;
    }

    void visit(CodeBlockElement* element) override {
        codeLineCount_ += countLines(element->getCode());
        elementCounts_["代码块"]++;
        cout << "    ├─ 代码 [" << element->getLanguage() << "] → "
             << countLines(element->getCode()) << " 行（不计入字数）" << endl;
    }

    void visit(LinkElement* element) override {
        int count = countWords(element->getText());
        wordCount_ += count;
        linkCount_++;
        elementCounts_["超链接"]++;
        cout << "    ├─ 链接 \"" << element->getText() << "\" → " << count << " 字" << endl;
    }

    string getVisitorName() const override { return "字数统计访问者"; }

    int getWordCount() const { return wordCount_; }
    int getCharCount() const { return charCount_; }
    int getImageCount() const { return imageCount_; }
    int getTableCount() const { return tableCount_; }
    int getLinkCount() const { return linkCount_; }
    int getCodeLineCount() const { return codeLineCount_; }
    const map<string, int>& getElementCounts() const { return elementCounts_; }

    void printSummary() const {
        cout << "    ┌─────────────────────────────────────┐" << endl;
        cout << "    │         文档统计报告                 │" << endl;
        cout << "    ├─────────────────────────────────────┤" << endl;
        cout << "    │ 总字数(含表格): " << wordCount_ << endl;
        cout << "    │ 总字符数:       " << charCount_ << endl;
        cout << "    │ 图片数量:       " << imageCount_ << endl;
        cout << "    │ 表格数量:       " << tableCount_ << endl;
        cout << "    │ 超链接数量:     " << linkCount_ << endl;
        cout << "    │ 代码行数:       " << codeLineCount_ << endl;
        cout << "    ├─────────────────────────────────────┤" << endl;
        cout << "    │ 元素分布:" << endl;
        for (const auto& pair : elementCounts_) {
            cout << "    │   " << pair.first << ": " << pair.second << " 个" << endl;
        }
        cout << "    └─────────────────────────────────────┘" << endl;
    }

private:
    int countWords(const string& text) {
        if (text.empty()) return 0;
        int count = 0;
        bool inWord = false;
        for (char c : text) {
            if (c == ' ' || c == '\n' || c == '\t') {
                if (inWord) count++;
                inWord = false;
            } else {
                inWord = true;
            }
        }
        if (inWord) count++;
        // 对于中文，每个字符算一个字
        int chineseCount = 0;
        for (unsigned char c : text) {
            if (c >= 0xE0) chineseCount++;  // 简化判断UTF-8中文字符
        }
        return max(count, chineseCount / 3);  // UTF-8中文占3字节
    }

    int countLines(const string& text) {
        if (text.empty()) return 0;
        return static_cast<int>(count(text.begin(), text.end(), '\n') + 1);
    }

    string truncate(const string& text, int maxLen) {
        if (static_cast<int>(text.size()) <= maxLen) return text;
        return text.substr(0, maxLen) + "...";
    }

    int wordCount_ = 0;
    int charCount_ = 0;
    int imageCount_ = 0;
    int tableCount_ = 0;
    int linkCount_ = 0;
    int codeLineCount_ = 0;
    map<string, int> elementCounts_;
};

// ======================== 具体访问者C：拼写检查访问者 ========================
class SpellCheckVisitor : public DocumentVisitor {
public:
    SpellCheckVisitor(const vector<string>& dictionary)
        : dictionary_(dictionary) {}

    void visit(TextElement* element) override {
        checkText(element->getContent(), element->getType());
    }

    void visit(ImageElement* element) override {
        if (!element->getCaption().empty()) {
            checkText(element->getCaption(), "图片说明");
        }
        cout << "    ├─ 图片 [" << element->getPath() << "] → 无需检查" << endl;
    }

    void visit(TableElement* element) override {
        for (const auto& header : element->getHeaders()) {
            checkText(header, "表头");
        }
        int rowIdx = 0;
        for (const auto& row : element->getRows()) {
            for (const auto& cell : row) {
                checkText(cell, "表格单元格[行" + to_string(rowIdx) + "]");
            }
            rowIdx++;
        }
    }

    void visit(CodeBlockElement* element) override {
        cout << "    ├─ 代码 [" << element->getLanguage() << "] → 跳过拼写检查" << endl;
    }

    void visit(LinkElement* element) override {
        checkText(element->getText(), "链接文本");
        // 检查URL格式
        if (element->getUrl().find("http") == string::npos) {
            errors_.push_back("链接URL格式异常: " + element->getUrl());
            cout << "    ├─ ✗ 链接URL格式异常: " << element->getUrl() << endl;
        }
    }

    string getVisitorName() const override { return "拼写检查访问者"; }

    bool hasErrors() const { return !errors_.empty(); }
    const vector<string>& getErrors() const { return errors_; }
    int getCheckedElements() const { return checkedCount_; }

    void printReport() const {
        cout << "    ┌─────────────────────────────────────┐" << endl;
        cout << "    │         拼写检查报告                 │" << endl;
        cout << "    ├─────────────────────────────────────┤" << endl;
        cout << "    │ 检查元素数: " << checkedCount_ << endl;
        cout << "    │ 发现问题数: " << errors_.size() << endl;
        if (errors_.empty()) {
            cout << "    │ 状态: ✓ 全部通过" << endl;
        } else {
            cout << "    │ 状态: ✗ 存在问题" << endl;
            cout << "    ├─────────────────────────────────────┤" << endl;
            cout << "    │ 问题详情:" << endl;
            for (size_t i = 0; i < errors_.size(); i++) {
                cout << "    │   " << (i + 1) << ". " << errors_[i] << endl;
            }
        }
        cout << "    └─────────────────────────────────────┘" << endl;
    }

private:
    void checkText(const string& text, const string& context) {
        checkedCount_++;
        // 简单的拼写检查模拟：检查常见错误
        vector<pair<string, string>> knownErrors = {
            {"teh", "the"}, {"recieve", "receive"}, {"occured", "occurred"},
            {"seperate", "separate"}, {"definately", "definitely"},
            {"wierd", "weird"}, {"accomodate", "accommodate"}
        };

        string lowerText = text;
        transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);

        bool foundError = false;
        for (const auto& err : knownErrors) {
            if (lowerText.find(err.first) != string::npos) {
                string msg = context + " 中发现拼写错误: \"" + err.first + "\" → 建议改为 \"" + err.second + "\"";
                errors_.push_back(msg);
                cout << "    ├─ ✗ " << msg << endl;
                foundError = true;
            }
        }

        if (!foundError) {
            if (text.size() > 30) {
                cout << "    ├─ ✓ " << context << " \"" << text.substr(0, 30) << "...\" 通过" << endl;
            } else {
                cout << "    ├─ ✓ " << context << " \"" << text << "\" 通过" << endl;
            }
        }
    }

    vector<string> dictionary_;
    vector<string> errors_;
    int checkedCount_ = 0;
};

// ======================== 具体访问者D：图片提取访问者 ========================
class ExtractImageVisitor : public DocumentVisitor {
public:
    void visit(TextElement* element) override {
        // 文本元素不含图片，跳过
    }

    void visit(ImageElement* element) override {
        images_.push_back({element->getPath(), element->getWidth(),
                          element->getHeight(), element->getCaption()});
        totalSize_ += element->getWidth() * element->getHeight();
        cout << "    ├─ 发现图片: " << element->getPath()
             << " (" << element->getWidth() << "×" << element->getHeight() << ")"
             << endl;
    }

    void visit(TableElement* element) override {
        // 表格中可能嵌套图片（简化处理）
    }

    void visit(CodeBlockElement* element) override {
        // 代码块不含图片
    }

    void visit(LinkElement* element) override {
        // 链接可能指向图片（简化处理）
        if (element->getUrl().find(".png") != string::npos ||
            element->getUrl().find(".jpg") != string::npos ||
            element->getUrl().find(".gif") != string::npos) {
            linkedImages_.push_back(element->getUrl());
            cout << "    ├─ 发现图片链接: " << element->getUrl() << endl;
        }
    }

    string getVisitorName() const override { return "图片提取访问者"; }

    struct ImageInfo {
        string path;
        int width;
        int height;
        string caption;
    };

    const vector<ImageInfo>& getImages() const { return images_; }
    const vector<string>& getLinkedImages() const { return linkedImages_; }
    int getTotalPixelArea() const { return totalSize_; }

    void printReport() const {
        cout << "    ┌─────────────────────────────────────┐" << endl;
        cout << "    │         图片提取报告                 │" << endl;
        cout << "    ├─────────────────────────────────────┤" << endl;
        cout << "    │ 内嵌图片数: " << images_.size() << endl;
        cout << "    │ 链接图片数: " << linkedImages_.size() << endl;
        cout << "    │ 总像素面积: " << totalSize_ << " px²" << endl;
        if (!images_.empty()) {
            cout << "    ├─────────────────────────────────────┤" << endl;
            cout << "    │ 图片清单:" << endl;
            for (size_t i = 0; i < images_.size(); i++) {
                cout << "    │   " << (i + 1) << ". " << images_[i].path
                     << " (" << images_[i].width << "×" << images_[i].height << ")";
                if (!images_[i].caption.empty()) {
                    cout << " - " << images_[i].caption;
                }
                cout << endl;
            }
        }
        if (!linkedImages_.empty()) {
            cout << "    ├─────────────────────────────────────┤" << endl;
            cout << "    │ 图片链接:" << endl;
            for (size_t i = 0; i < linkedImages_.size(); i++) {
                cout << "    │   " << (i + 1) << ". " << linkedImages_[i] << endl;
            }
        }
        cout << "    └─────────────────────────────────────┘" << endl;
    }

private:
    vector<ImageInfo> images_;
    vector<string> linkedImages_;
    int totalSize_ = 0;
};

// ======================== 具体访问者E：目录生成访问者 ========================
class TOCVisitor : public DocumentVisitor {
public:
    void visit(TextElement* element) override {
        if (element->getStyle() == "heading1") {
            h1Count_++;
            currentH1_ = to_string(h1Count_);
            tocEntries_.push_back({1, currentH1_ + ". " + element->getContent()});
            cout << "    ├─ H1: " << currentH1_ << ". " << element->getContent() << endl;
        } else if (element->getStyle() == "heading2") {
            h2Count_++;
            string entry = currentH1_ + "." + to_string(h2Count_);
            tocEntries_.push_back({2, entry + ". " + element->getContent()});
            cout << "    ├─ H2:   " << entry << ". " << element->getContent() << endl;
        }
    }

    void visit(ImageElement* element) override {
        if (!element->getCaption().empty()) {
            figureCount_++;
            tocEntries_.push_back({3, "图" + to_string(figureCount_) + ": " + element->getCaption()});
        }
    }

    void visit(TableElement* element) override {
        tableCount_++;
        tocEntries_.push_back({3, "表" + to_string(tableCount_) + ": (" 
                              + to_string(element->getRowCount()) + "行×"
                              + to_string(element->getColCount()) + "列)"});
    }

    void visit(CodeBlockElement* element) override {
        codeCount_++;
        tocEntries_.push_back({3, "代码清单" + to_string(codeCount_) + " [" + element->getLanguage() + "]"});
    }

    void visit(LinkElement* element) override {
        // 链接不纳入目录
    }

    string getVisitorName() const override { return "目录生成访问者"; }

    const vector<pair<int, string>>& getTOCEntries() const { return tocEntries_; }

    void printTOC() const {
        cout << "    ┌─────────────────────────────────────┐" << endl;
        cout << "    │         文档目录                     │" << endl;
        cout << "    ├─────────────────────────────────────┤" << endl;
        for (const auto& entry : tocEntries_) {
            string indent = "";
            if (entry.first == 2) indent = "    ";
            else if (entry.first == 3) indent = "        ";
            cout << "    │ " << indent << entry.second << endl;
        }
        cout << "    ├─────────────────────────────────────┤" << endl;
        cout << "    │ 章节统计: H1=" << h1Count_ << " H2=" << h2Count_ << endl;
        cout << "    │ 图: " << figureCount_ << " 表: " << tableCount_
             << " 代码: " << codeCount_ << endl;
        cout << "    └─────────────────────────────────────┘" << endl;
    }

private:
    vector<pair<int, string>> tocEntries_;  // level, text
    int h1Count_ = 0;
    int h2Count_ = 0;
    int figureCount_ = 0;
    int tableCount_ = 0;
    int codeCount_ = 0;
    string currentH1_;
};

// ======================== 辅助函数 ========================
void printSection(const string& title) {
    cout << endl;
    cout << "  ┌─ " << title << " ─────────────────────────────" << endl;
}

void printSectionEnd() {
    cout << "  └──────────────────────────────────────────────" << endl;
}

shared_ptr<Document> buildSampleDocument() {
    auto doc = make_shared<Document>("C++设计模式教程");

    doc->addElement(make_shared<TextElement>("C++设计模式教程", "heading1"));
    doc->addElement(make_shared<TextElement>("设计模式是软件开发中经过验证的最佳实践，本文将介绍几种常用的设计模式。"));

    doc->addElement(make_shared<TextElement>("创建型模式", "heading2"));
    doc->addElement(make_shared<TextElement>("创建型模式关注对象的创建机制，试图以适合情况的方式创建对象。"));
    doc->addElement(make_shared<ImageElement>("/images/creational_patterns.png", 800, 600, "创建型模式分类图"));
    doc->addElement(make_shared<LinkElement>("了解更多创建型模式", "https://refactoring.guru/design-patterns/creational-patterns"));

    doc->addElement(make_shared<TextElement>("结构型模式", "heading2"));
    doc->addElement(make_shared<TextElement>("结构型模式关注如何将类或对象组合成更大的结构。"));
    doc->addElement(make_shared<TableElement>(
        vector<string>{"模式名称", "用途", "复杂度"},
        vector<vector<string>>{
            {"Adapter", "接口适配", "低"},
            {"Bridge", "分离抽象与实现", "中"},
            {"Composite", "树形结构", "中"},
            {"Decorator", "动态扩展", "中"},
            {"Proxy", "代理控制", "低"}
        }
    ));

    doc->addElement(make_shared<TextElement>("行为型模式", "heading2"));
    doc->addElement(make_shared<TextElement>("行为型模式关注对象之间的职责分配和通信。"));
    doc->addElement(make_shared<CodeBlockElement>(
        "// 观察者模式示例\nclass Observer {\npublic:\n    virtual void update(const string& msg) = 0;\n};",
        "cpp"
    ));
    doc->addElement(make_shared<LinkElement>("行为型模式详解", "https://refactoring.guru/design-patterns/behavioral-patterns"));

    doc->addElement(make_shared<TextElement>("本文介绍了seperate的创建型模式和teh结构型模式的基本概念。"));

    return doc;
}

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "    访问者模式演示 - 文档处理系统       " << endl;
    cout << "========================================" << endl;

    // ---------- 构建文档 ----------
    printSection("构建文档结构");
    auto doc = buildSampleDocument();
    cout << "    文档: " << doc->getTitle() << endl;
    cout << "    元素总数: " << doc->getElements().size() << endl;
    cout << "    元素类型: 文本段落、图片、表格、代码块、超链接" << endl;
    printSectionEnd();

    // ---------- 1. HTML导出访问者 ----------
    cout << endl;
    cout << ">>> 场景一：将文档导出为HTML <<<" << endl;

    printSection("执行HTML导出");
    auto htmlVisitor = make_unique<ExportHTMLVisitor>();
    doc->accept(htmlVisitor.get());
    printSectionEnd();

    printSection("生成的HTML片段");
    string html = htmlVisitor->getHTML();
    // 只显示前500字符
    if (html.size() > 500) {
        cout << html.substr(0, 500) << endl;
        cout << "    ... (共 " << html.size() << " 字符)" << endl;
    } else {
        cout << html << endl;
    }
    printSectionEnd();

    // ---------- 2. 字数统计访问者 ----------
    cout << endl;
    cout << ">>> 场景二：统计文档字数 <<<" << endl;

    printSection("执行字数统计");
    auto wordCountVisitor = make_unique<WordCountVisitor>();
    doc->accept(wordCountVisitor.get());
    printSectionEnd();

    printSection("统计结果");
    wordCountVisitor->printSummary();
    printSectionEnd();

    // ---------- 3. 拼写检查访问者 ----------
    cout << endl;
    cout << ">>> 场景三：文档拼写检查 <<<" << endl;

    printSection("执行拼写检查");
    vector<string> dictionary = {"design", "pattern", "cpp", "observer", "adapter"};
    auto spellCheckVisitor = make_unique<SpellCheckVisitor>(dictionary);
    doc->accept(spellCheckVisitor.get());
    printSectionEnd();

    printSection("检查报告");
    spellCheckVisitor->printReport();
    printSectionEnd();

    // ---------- 4. 图片提取访问者 ----------
    cout << endl;
    cout << ">>> 场景四：提取文档中的图片资源 <<<" << endl;

    printSection("执行图片提取");
    auto imageVisitor = make_unique<ExtractImageVisitor>();
    doc->accept(imageVisitor.get());
    printSectionEnd();

    printSection("提取报告");
    imageVisitor->printReport();
    printSectionEnd();

    // ---------- 5. 目录生成访问者 ----------
    cout << endl;
    cout << ">>> 场景五：生成文档目录 <<<" << endl;

    printSection("扫描文档结构");
    auto tocVisitor = make_unique<TOCVisitor>();
    doc->accept(tocVisitor.get());
    printSectionEnd();

    printSection("生成目录");
    tocVisitor->printTOC();
    printSectionEnd();

    // ---------- 6. 展示访问者模式的优势 ----------
    cout << endl;
    cout << ">>> 访问者模式优势总结 <<<" << endl;
    cout << "  ├─ 操作分离：将操作从元素类中抽离，封装到独立的访问者中" << endl;
    cout << "  ├─ 开闭原则：新增操作只需添加新访问者，无需修改元素类" << endl;
    cout << "  ├─ 双重分派：通过 accept() + visit() 实现运行时多态分派" << endl;
    cout << "  ├─ 累积状态：访问者可以在遍历过程中累积状态（如字数统计）" << endl;
    cout << "  ├─ 集中相关操作：同一访问者中的操作逻辑内聚，便于维护" << endl;
    cout << "  ├─ 适用场景：对象结构稳定、操作频繁变化的系统" << endl;
    cout << "  └─ 注意事项：新增元素类型需要修改所有访问者（违反开闭原则）" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}