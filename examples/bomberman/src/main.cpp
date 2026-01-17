#include <vinter/engine.hpp>
#include <vinter/settings/project_settings.hpp>

class Bomberman : public vn::Engine {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings)
        : Engine(project_settings) {
    }

protected:
    void load() override {}
    void poll_events() override { Engine::poll_events(); }
    void update() override {}
    void render() override {}
};

int main() {
    const vn::ProjectSettings project_settings {};
    Bomberman bomberman(project_settings);
    bomberman.run();

    return 0;
}