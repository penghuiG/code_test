/**
 * 适配器模式 (Adapter Pattern)
 * 
 * 将一个类的接口转换成客户希望的另一个接口。适配器模式使得原本由于接口不兼容
 * 而不能一起工作的那些类可以一起工作。
 * 适配器模式分为两种：
 *   - 类适配器（通过多重继承实现）
 *   - 对象适配器（通过组合实现，更灵活、更常用）
 * 
 * 本例以多媒体播放器为场景，演示对象适配器模式的核心结构：
 *   - Target（目标接口，客户期望的接口）
 *   - Adaptee（被适配者，已有的不兼容接口）
 *   - Adapter（适配器，将 Adaptee 的接口转换为 Target 接口）
 * 
 * 场景说明：
 *   现有一个音乐播放器可以直接播放 mp3 文件，但系统还需要播放
 *   其他格式（vlc、mp4、avi）的文件。已有的第三方库提供了各自
 *   不同的接口来播放这些格式。通过适配器模式，将这些不同接口
 *   统一适配到同一个 MediaPlayer 接口上。
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

// ======================== 目标接口：统一的媒体播放器 ========================
class MediaPlayer {
public:
    virtual ~MediaPlayer() {}
    virtual void play(const string& filename) = 0;
    virtual void stop() = 0;
    virtual string getFormat() const = 0;
};

// ======================== 已有实现：MP3 播放器（直接符合接口） ========================
class Mp3Player : public MediaPlayer {
public:
    void play(const string& filename) override {
        cout << "  [MP3 播放器] 正在播放: " << filename << ".mp3" << endl;
        cout << "  ▶♪♪ ♪♪ ♪♪  解码 MP3 音频流..." << endl;
        playing_ = filename;
    }

    void stop() override {
        if (!playing_.empty()) {
            cout << "  [MP3 播放器] 停止播放: " << playing_ << ".mp3" << endl;
            playing_.clear();
        }
    }

    string getFormat() const override { return "MP3"; }

private:
    string playing_;
};

// ======================== 第三方库：VLC 播放器（不兼容的接口） ========================
class VlcLibrary {
public:
    void openVlcFile(const string& url) {
        cout << "  [VLC 第三方库] 打开 VLC 文件: " << url << ".vlc" << endl;
    }

    void playVlcMedia() {
        cout << "  ▶♫♫ ♫♫ ♫♫  VLC 引擎解码中..." << endl;
    }

    void closeVlcFile() {
        cout << "  [VLC 第三方库] 关闭 VLC 文件" << endl;
    }
};

// ======================== 第三方库：MP4 播放器（不兼容的接口） ========================
class Mp4Decoder {
public:
    void loadMp4(const string& filepath) {
        cout << "  [MP4 解码器] 加载文件: " << filepath << ".mp4" << endl;
    }

    void decodeAndPlay() {
        cout << "  ▶►► ►► ►►  MP4 硬件加速解码..." << endl;
    }

    void unloadMp4() {
        cout << "  [MP4 解码器] 释放 MP4 资源" << endl;
    }
};

// ======================== 第三方库：AVI 播放器（不兼容的接口） ========================
class AviPlayer {
public:
    void startAviPlayback(const string& name) {
        cout << "  [AVI 播放组件] 开始播放: " << name << ".avi" << endl;
        cout << "  ▶■■ ■■ ■■  AVI 流媒体渲染..." << endl;
    }

    void stopAviPlayback() {
        cout << "  [AVI 播放组件] 停止 AVI 播放" << endl;
    }
};

// ======================== 适配器：将 VLC 适配为 MediaPlayer ========================
class VlcAdapter : public MediaPlayer {
public:
    VlcAdapter(shared_ptr<VlcLibrary> vlc) : vlc_(vlc), playing_(false) {}

    void play(const string& filename) override {
        cout << "  [VLC 适配器] 转换接口..." << endl;
        vlc_->openVlcFile(filename);
        vlc_->playVlcMedia();
        playing_ = true;
        currentFile_ = filename;
    }

    void stop() override {
        if (playing_) {
            vlc_->closeVlcFile();
            playing_ = false;
            currentFile_.clear();
        }
    }

    string getFormat() const override { return "VLC"; }

private:
    shared_ptr<VlcLibrary> vlc_;
    bool playing_;
    string currentFile_;
};

// ======================== 适配器：将 MP4 适配为 MediaPlayer ========================
class Mp4Adapter : public MediaPlayer {
public:
    Mp4Adapter(shared_ptr<Mp4Decoder> decoder) : decoder_(decoder), playing_(false) {}

    void play(const string& filename) override {
        cout << "  [MP4 适配器] 转换接口..." << endl;
        decoder_->loadMp4(filename);
        decoder_->decodeAndPlay();
        playing_ = true;
        currentFile_ = filename;
    }

    void stop() override {
        if (playing_) {
            decoder_->unloadMp4();
            playing_ = false;
            currentFile_.clear();
        }
    }

    string getFormat() const override { return "MP4"; }

private:
    shared_ptr<Mp4Decoder> decoder_;
    bool playing_;
    string currentFile_;
};

// ======================== 适配器：将 AVI 适配为 MediaPlayer ========================
class AviAdapter : public MediaPlayer {
public:
    AviAdapter(shared_ptr<AviPlayer> avi) : avi_(avi), playing_(false) {}

    void play(const string& filename) override {
        cout << "  [AVI 适配器] 转换接口..." << endl;
        avi_->startAviPlayback(filename);
        playing_ = true;
        currentFile_ = filename;
    }

    void stop() override {
        if (playing_) {
            avi_->stopAviPlayback();
            playing_ = false;
            currentFile_.clear();
        }
    }

    string getFormat() const override { return "AVI"; }

private:
    shared_ptr<AviPlayer> avi_;
    bool playing_;
    string currentFile_;
};

// ======================== 客户端使用的统一媒体播放器管理类 ========================
class MediaPlayerManager {
public:
    void addPlayer(shared_ptr<MediaPlayer> player) {
        cout << "[媒体管理器] 添加 " << player->getFormat() << " 格式支持" << endl;
        players_.push_back(player);
    }

    // 使用指定的播放器播放文件
    void playWith(shared_ptr<MediaPlayer> player, const string& filename) {
        cout << "[媒体管理器] 播放文件: " << filename
             << " (" << player->getFormat() << " 格式)" << endl;
        player->play(filename);
    }

    // 停止指定播放器
    void stopWith(shared_ptr<MediaPlayer> player) {
        cout << "[媒体管理器] 停止 " << player->getFormat() << " 播放器" << endl;
        player->stop();
    }

    // 显示已支持的格式
    void showSupportedFormats() const {
        cout << "[媒体管理器] 已支持的格式:" << endl;
        for (const auto& p : players_) {
            cout << "  ✓ " << p->getFormat() << endl;
        }
    }

private:
    vector<shared_ptr<MediaPlayer>> players_;
};

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "     适配器模式演示 - 多媒体播放器       " << endl;
    cout << "========================================" << endl;

    // 创建媒体管理器
    MediaPlayerManager manager;

    // ---------- 1. MP3 播放器直接符合接口，无需适配 ----------
    cout << endl;
    cout << ">>> 步骤一：添加 MP3 播放器（原生支持） <<<" << endl;
    auto mp3Player = make_shared<Mp3Player>();
    manager.addPlayer(mp3Player);

    // ---------- 2. 为第三方库创建适配器 ----------
    cout << endl;
    cout << ">>> 步骤二：为第三方库创建适配器 <<<" << endl;

    auto vlcLib = make_shared<VlcLibrary>();
    auto vlcAdapter = make_shared<VlcAdapter>(vlcLib);
    manager.addPlayer(vlcAdapter);

    auto mp4Decoder = make_shared<Mp4Decoder>();
    auto mp4Adapter = make_shared<Mp4Adapter>(mp4Decoder);
    manager.addPlayer(mp4Adapter);

    auto aviPlayer = make_shared<AviPlayer>();
    auto aviAdapter = make_shared<AviAdapter>(aviPlayer);
    manager.addPlayer(aviAdapter);

    // ---------- 3. 显示支持的格式 ----------
    cout << endl;
    cout << ">>> 步骤三：显示支持的格式 <<<" << endl;
    manager.showSupportedFormats();

    // ---------- 4. 统一接口播放不同格式 ----------
    cout << endl;
    cout << ">>> 步骤四：使用统一接口播放不同格式 <<<" << endl;

    cout << endl;
    manager.playWith(mp3Player, "青花瓷");
    cout << endl;
    manager.stopWith(mp3Player);

    cout << endl;
    manager.playWith(vlcAdapter, "黑客帝国");
    cout << endl;
    manager.stopWith(vlcAdapter);

    cout << endl;
    manager.playWith(mp4Adapter, "流浪地球");
    cout << endl;
    manager.stopWith(mp4Adapter);

    cout << endl;
    manager.playWith(aviAdapter, "教父");
    cout << endl;
    manager.stopWith(aviAdapter);

    // ---------- 5. 演示适配器的关键价值：统一接口 ----------
    cout << endl;
    cout << ">>> 步骤五：通过统一接口批量播放 <<<" << endl;
    vector<pair<shared_ptr<MediaPlayer>, string>> playlist = {
        {mp3Player,   "夜曲"},
        {vlcAdapter,  "肖申克的救赎"},
        {mp4Adapter,  "星际穿越"},
        {aviAdapter,  "阿甘正传"}
    };

    for (size_t i = 0; i < playlist.size(); ++i) {
        cout << endl;
        cout << "  ── 播放第 " << (i + 1) << " 首 ──" << endl;
        playlist[i].first->play(playlist[i].second);
        playlist[i].first->stop();
    }

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}