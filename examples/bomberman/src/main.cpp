#include <vinter/engine.hpp>

class Bomberman : public vn::Engine {
protected:
    void load() override {}
    void poll_events() override { Engine::poll_events(); }
    void update() override {}
    void render() override {}
};

int main() {
    Bomberman bomberman;
    bomberman.run();

    return 0;
}