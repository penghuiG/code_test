/**
 * 外观模式 (Facade Pattern)
 * 
 * 为子系统中的一组接口提供一个一致的界面。外观模式定义了一个高层接口，
 * 这个接口使得这一子系统更加容易使用。外观模式通过引入一个外观类，
 * 将复杂的子系统操作封装起来，客户端只需要与外观类交互即可。
 * 
 * 本例以家庭影院为场景，演示外观模式的核心结构：
 *   - Facade（外观类）：家庭影院外观（HomeTheaterFacade），为子系统提供统一的高层接口
 *   - Subsystem（子系统类）：各个设备（投影仪、音响、DVD播放器、灯光、爆米花机等），
 *     实现子系统的实际功能，处理外观对象指派的工作
 *   - Client（客户端）：通过外观接口与子系统交互，无需了解子系统的内部细节
 * 
 * 场景说明：
 *   家庭影院包含多个子系统设备（投影仪、功放、DVD播放器、灯光、幕布、爆米花机等）。
 *   看电影时需要依次操作这些设备，步骤繁琐且容易出错。
 *   通过外观模式，用户只需调用"开始观影"和"结束观影"两个方法，外观类会协调所有设备。
 */

#include <iostream>
#include <memory>
#include <string>
using namespace std;

// ======================== 子系统：投影仪 ========================
class Projector {
public:
    void on() {
        cout << "    📽️  投影仪：开机预热中..." << endl;
    }

    void off() {
        cout << "    📽️  投影仪：关闭" << endl;
    }

    void setInput(const string& input) {
        cout << "    📽️  投影仪：切换输入源为 " << input << endl;
    }

    void setWideScreenMode() {
        cout << "    📽️  投影仪：设置宽屏模式 (16:9)" << endl;
    }

    void setBrightness(int level) {
        cout << "    📽️  投影仪：亮度调整为 " << level << "%" << endl;
    }
};

// ======================== 子系统：功放（音频设备） ========================
class Amplifier {
public:
    void on() {
        cout << "    🔊 功放：开机" << endl;
    }

    void off() {
        cout << "    🔊 功放：关机" << endl;
    }

    void setVolume(int level) {
        cout << "    🔊 功放：音量设置为 " << level << endl;
    }

    void setSurroundSound() {
        cout << "    🔊 功放：开启环绕立体声模式" << endl;
    }

    void setStereo() {
        cout << "    🔊 功放：切换为立体声模式" << endl;
    }
};

// ======================== 子系统：DVD播放器 ========================
class DvdPlayer {
public:
    void on() {
        cout << "    💿 DVD播放器：开机" << endl;
    }

    void off() {
        cout << "    💿 DVD播放器：关机" << endl;
    }

    void play(const string& movie) {
        cout << "    💿 DVD播放器：正在播放《" << movie << "》" << endl;
    }

    void pause() {
        cout << "    💿 DVD播放器：暂停播放" << endl;
    }

    void stop() {
        cout << "    💿 DVD播放器：停止播放" << endl;
    }

    void eject() {
        cout << "    💿 DVD播放器：弹出光盘" << endl;
    }
};

// ======================== 子系统：灯光 ========================
class Lights {
public:
    void on() {
        cout << "    💡 灯光：全开" << endl;
    }

    void off() {
        cout << "    💡 灯光：关闭" << endl;
    }

    void dim(int level) {
        cout << "    💡 灯光：调暗至 " << level << "%" << endl;
    }
};

// ======================== 子系统：幕布 ========================
class Screen {
public:
    void up() {
        cout << "    🖥️  幕布：升起" << endl;
    }

    void down() {
        cout << "    🖥️  幕布：降下" << endl;
    }
};

// ======================== 子系统：爆米花机 ========================
class PopcornPopper {
public:
    void on() {
        cout << "    🍿 爆米花机：启动加热" << endl;
    }

    void off() {
        cout << "    🍿 爆米花机：关闭" << endl;
    }

    void pop() {
        cout << "    🍿 爆米花机：正在制作爆米花，噼里啪啦~" << endl;
    }
};

// ======================== 子系统：流媒体播放器（额外设备） ========================
class StreamingPlayer {
public:
    void on() {
        cout << "    📱 流媒体播放器：开机连接" << endl;
    }

    void off() {
        cout << "    📱 流媒体播放器：断开连接" << endl;
    }

    void play(const string& movie) {
        cout << "    📱 流媒体播放器：正在流式播放《" << movie << "》" << endl;
    }

    void stop() {
        cout << "    📱 流媒体播放器：停止播放" << endl;
    }
};

// ======================== 外观类：家庭影院外观 ========================
class HomeTheaterFacade {
public:
    HomeTheaterFacade(
        shared_ptr<Projector> projector,
        shared_ptr<Amplifier> amplifier,
        shared_ptr<DvdPlayer> dvdPlayer,
        shared_ptr<Lights> lights,
        shared_ptr<Screen> screen,
        shared_ptr<PopcornPopper> popcornPopper,
        shared_ptr<StreamingPlayer> streamingPlayer
    )
        : projector_(projector)
        , amplifier_(amplifier)
        , dvdPlayer_(dvdPlayer)
        , lights_(lights)
        , screen_(screen)
        , popcornPopper_(popcornPopper)
        , streamingPlayer_(streamingPlayer)
    {}

    // 外观接口1：使用DVD播放电影
    void watchMovie(const string& movie) {
        cout << endl;
        cout << "  ╔══════════════════════════════════════╗" << endl;
        cout << "  ║   🎬 准备观影：" << movie << endl;
        cout << "  ╚══════════════════════════════════════╝" << endl;

        cout << endl;
        cout << "  [1/7] 准备爆米花..." << endl;
        popcornPopper_->on();
        popcornPopper_->pop();

        cout << endl;
        cout << "  [2/7] 灯光调暗..." << endl;
        lights_->dim(10);

        cout << endl;
        cout << "  [3/7] 降下幕布..." << endl;
        screen_->down();

        cout << endl;
        cout << "  [4/7] 启动投影仪..." << endl;
        projector_->on();
        projector_->setWideScreenMode();
        projector_->setInput("DVD");
        projector_->setBrightness(80);

        cout << endl;
        cout << "  [5/7] 启动功放..." << endl;
        amplifier_->on();
        amplifier_->setSurroundSound();
        amplifier_->setVolume(7);

        cout << endl;
        cout << "  [6/7] 启动DVD播放器..." << endl;
        dvdPlayer_->on();
        dvdPlayer_->play(movie);

        cout << endl;
        cout << "  [7/7] 关闭灯光..." << endl;
        lights_->off();

        cout << endl;
        cout << "  ✅ 一切就绪，开始享受电影吧！" << endl;
    }

    // 外观接口2：结束观影（使用DVD）
    void endMovie() {
        cout << endl;
        cout << "  ╔══════════════════════════════════════╗" << endl;
        cout << "  ║   🎬 结束观影，关闭设备              ║" << endl;
        cout << "  ╚══════════════════════════════════════╝" << endl;

        cout << endl;
        cout << "  [1/6] 停止DVD播放器..." << endl;
        dvdPlayer_->stop();
        dvdPlayer_->eject();
        dvdPlayer_->off();

        cout << endl;
        cout << "  [2/6] 关闭功放..." << endl;
        amplifier_->off();

        cout << endl;
        cout << "  [3/6] 关闭投影仪..." << endl;
        projector_->off();

        cout << endl;
        cout << "  [4/6] 升起幕布..." << endl;
        screen_->up();

        cout << endl;
        cout << "  [5/6] 开灯..." << endl;
        lights_->on();

        cout << endl;
        cout << "  [6/6] 关闭爆米花机..." << endl;
        popcornPopper_->off();

        cout << endl;
        cout << "  ✅ 所有设备已关闭，感谢观影！" << endl;
    }

    // 外观接口3：使用流媒体播放电影
    void watchStreaming(const string& movie) {
        cout << endl;
        cout << "  ╔══════════════════════════════════════╗" << endl;
        cout << "  ║   📺 准备流媒体观影：" << movie << endl;
        cout << "  ╚══════════════════════════════════════╝" << endl;

        cout << endl;
        cout << "  [1/6] 灯光调暗..." << endl;
        lights_->dim(15);

        cout << endl;
        cout << "  [2/6] 降下幕布..." << endl;
        screen_->down();

        cout << endl;
        cout << "  [3/6] 启动投影仪..." << endl;
        projector_->on();
        projector_->setWideScreenMode();
        projector_->setInput("HDMI");
        projector_->setBrightness(85);

        cout << endl;
        cout << "  [4/6] 启动功放..." << endl;
        amplifier_->on();
        amplifier_->setStereo();
        amplifier_->setVolume(5);

        cout << endl;
        cout << "  [5/6] 启动流媒体播放器..." << endl;
        streamingPlayer_->on();
        streamingPlayer_->play(movie);

        cout << endl;
        cout << "  [6/6] 关闭灯光..." << endl;
        lights_->off();

        cout << endl;
        cout << "  ✅ 流媒体就绪，开始享受电影吧！" << endl;
    }

    // 外观接口4：结束流媒体观影
    void endStreaming() {
        cout << endl;
        cout << "  ╔══════════════════════════════════════╗" << endl;
        cout << "  ║   📺 结束流媒体观影，关闭设备        ║" << endl;
        cout << "  ╚══════════════════════════════════════╝" << endl;

        cout << endl;
        cout << "  [1/5] 停止流媒体播放器..." << endl;
        streamingPlayer_->stop();
        streamingPlayer_->off();

        cout << endl;
        cout << "  [2/5] 关闭功放..." << endl;
        amplifier_->off();

        cout << endl;
        cout << "  [3/5] 关闭投影仪..." << endl;
        projector_->off();

        cout << endl;
        cout << "  [4/5] 升起幕布，开灯..." << endl;
        screen_->up();
        lights_->on();

        cout << endl;
        cout << "  ✅ 所有设备已关闭！" << endl;
    }

private:
    shared_ptr<Projector> projector_;
    shared_ptr<Amplifier> amplifier_;
    shared_ptr<DvdPlayer> dvdPlayer_;
    shared_ptr<Lights> lights_;
    shared_ptr<Screen> screen_;
    shared_ptr<PopcornPopper> popcornPopper_;
    shared_ptr<StreamingPlayer> streamingPlayer_;
};

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "     外观模式演示 - 家庭影院控制系统     " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 创建子系统设备 ----------
    cout << endl;
    cout << ">>> 初始化：创建所有子系统设备 <<<" << endl;
    cout << endl;

    auto projector = make_shared<Projector>();
    auto amplifier = make_shared<Amplifier>();
    auto dvdPlayer = make_shared<DvdPlayer>();
    auto lights = make_shared<Lights>();
    auto screen = make_shared<Screen>();
    auto popcornPopper = make_shared<PopcornPopper>();
    auto streamingPlayer = make_shared<StreamingPlayer>();

    cout << "  已创建设备：投影仪、功放、DVD播放器、灯光、幕布、爆米花机、流媒体播放器" << endl;

    // ---------- 2. 创建外观对象 ----------
    cout << endl;
    cout << ">>> 创建家庭影院外观 <<<" << endl;

    auto homeTheater = make_shared<HomeTheaterFacade>(
        projector, amplifier, dvdPlayer, lights, screen, popcornPopper, streamingPlayer
    );

    // ---------- 3. 场景一：使用外观播放DVD电影 ----------
    cout << endl;
    cout << "════════════════════════════════════════" << endl;
    cout << "  场景一：通过外观接口播放DVD电影" << endl;
    cout << "════════════════════════════════════════" << endl;

    homeTheater->watchMovie("盗梦空间");

    cout << endl;
    cout << "  ... 电影播放中，享受精彩剧情 ..." << endl;
    cout << "  ... 按下暂停键 ..." << endl;
    cout << endl;
    cout << "  >>> 暂时不需要了，关闭影院 <<<" << endl;

    homeTheater->endMovie();

    // ---------- 4. 场景二：使用外观播放流媒体电影 ----------
    cout << endl;
    cout << "════════════════════════════════════════" << endl;
    cout << "  场景二：通过外观接口播放流媒体电影" << endl;
    cout << "════════════════════════════════════════" << endl;

    homeTheater->watchStreaming("星际穿越");

    cout << endl;
    cout << "  ... 流媒体播放中，沉浸在太空之旅 ..." << endl;
    cout << endl;
    cout << "  >>> 观影结束 <<<" << endl;

    homeTheater->endStreaming();

    // ---------- 5. 对比：不使用外观模式的客户端调用 ----------
    cout << endl;
    cout << "════════════════════════════════════════" << endl;
    cout << "  对比：不使用外观模式（手动操作所有设备）" << endl;
    cout << "════════════════════════════════════════" << endl;
    cout << endl;
    cout << "  如果没有外观类，客户端需要这样调用：" << endl;
    cout << endl;

    cout << "  // --- 手动开启所有设备 ---" << endl;
    popcornPopper->on();
    popcornPopper->pop();
    lights->dim(10);
    screen->down();
    projector->on();
    projector->setWideScreenMode();
    projector->setInput("DVD");
    projector->setBrightness(80);
    amplifier->on();
    amplifier->setSurroundSound();
    amplifier->setVolume(7);
    dvdPlayer->on();
    dvdPlayer->play("盗梦空间");
    lights->off();

    cout << endl;
    cout << "  ⚠️  需要记住所有设备的操作顺序和参数！" << endl;
    cout << "  ⚠️  如果设备增加或减少，所有客户端代码都需要修改！" << endl;

    // ---------- 6. 展示外观模式的价值 ----------
    cout << endl;
    cout << ">>> 外观模式优势总结 <<<" << endl;
    cout << "  ├─ 简化接口：客户端只需调用外观类的高层方法" << endl;
    cout << "  │  （如 watchMovie / endMovie 一键操作）" << endl;
    cout << "  ├─ 解耦合：客户端与子系统之间松耦合" << endl;
    cout << "  │  （客户端不需要知道投影仪、功放等具体细节）" << endl;
    cout << "  ├─ 层次定义：在复杂系统中引入层次结构" << endl;
    cout << "  │  （外观作为子系统的入口，便于分层管理）" << endl;
    cout << "  ├─ 降低依赖：减少客户端对子系统的直接依赖" << endl;
    cout << "  │  （子系统变化只影响外观类，不影响客户端）" << endl;
    cout << "  └─ 开闭原则：新增子系统功能时，可以在外观类中" << endl;
    cout << "     添加新的便捷方法，而不修改已有接口" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}