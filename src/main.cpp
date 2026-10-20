#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <deque>

using namespace geode::prelude;

namespace gdinput {
    struct Stage {
        int regular;
        int wave;
    };

    // One physics tick is treated as 1/240 second.
    constexpr Stage STAGES[] = {
        {2, 2},
        {4, 4},
        {8, 8},
        {16, 8}
    };
    constexpr int OFF = 4;
    inline int stage = 0;

    inline bool verbose() {
        return Mod::get()->getSettingValue<bool>("verbose-log");
    }

    inline void logStage() {
        if (stage == OFF) {
            log::info("[GDInput] Input buffer OFF (raw input)");
            return;
        }
        const auto s = STAGES[stage];
        log::info("[GDInput] Stage {}: {} ticks regular / {} ticks wave",
            stage + 1, s.regular, s.wave);
    }
}

struct GDInputPending {
    bool down;
    int button;
    bool player1;
    int remaining;
};

class $modify(GDInputBaseLayer, GJBaseGameLayer) {
    struct Fields {
        std::deque<GDInputPending> queue;
        bool applying = false;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        auto* fields = m_fields.self();

        if (fields->applying || gdinput::stage == gdinput::OFF) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            return;
        }

        auto* player = isPlayer1 ? m_player1 : m_player2;
        const bool wave = player && player->m_isDart;
        const auto config = gdinput::STAGES[gdinput::stage];
        const int delay = wave ? config.wave : config.regular;

        fields->queue.push_back({down, button, isPlayer1, delay});

        if (gdinput::verbose()) {
            log::info("[GDInput] Queued {} for {} ticks",
                down ? "press" : "release", delay);
        }
    }

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        auto* fields = m_fields.self();

        for (auto it = fields->queue.begin(); it != fields->queue.end();) {
            --it->remaining;
            if (it->remaining <= 0) {
                fields->applying = true;
                GJBaseGameLayer::handleButton(it->down, it->button, it->player1);
                fields->applying = false;
                if (gdinput::verbose()) {
                    log::info("[GDInput] Applied buffered input");
                }
                it = fields->queue.erase(it);
            } else {
                ++it;
            }
        }

        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
    }
};

class $modify(GDInputPlayLayer, PlayLayer) {
    void resetLevel() {
        auto* base = static_cast<GDInputBaseLayer*>(
            static_cast<GJBaseGameLayer*>(this)
        );
        base->m_fields->queue.clear();
        PlayLayer::resetLevel();
    }
};

class $modify(GDInputKeyboard, CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat, double timestamp) {
        if (down && !repeat && key == KEY_F6) {
            gdinput::stage = (gdinput::stage + 1) % 5;
            gdinput::logStage();
            return true;
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, timestamp);
    }
};

$on_mod(Loaded) {
    log::info("[GDInput] Loaded. F6 cycles 2 -> 4 -> 8 -> 16 -> OFF ticks.");
}
