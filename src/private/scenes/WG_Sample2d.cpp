
#include <memory>

#include "WG_EntityManager.h"
#include "componentdecorators/WG_Tickable.h"
#include "components/WG_Rect2d.h"
#include "components/WG_Transform.h"
#include "components/WG_UiRenderable.h"
#include "scenes/WG_SampleScenes.h"
#include "WG_Macros.h"


void clippingSamples(std::shared_ptr<WG_Engine> engine)
{
    std::shared_ptr<WG_EntityManager> em = engine->GetEntityManager();

    em->MakeChild<WG_Rect2d>("box_0")->SetProps({
        {"position", "200, 100"},
        {"rect-dimensions", "200, 100"},
        {"rect-color", "black"},
        {"clip-content", "true"},
        {"border-thickness", "2"},
        {"border-color", "orange"},
        })->WithChildren({
            em->MakeEntity<WG_Rect2d>("box_1a")->SetProps({
                {"position", "10, 10"},
                {"rect-dimensions", "90, 80"},
                {"rect-color", "red"},
                {"clip-content", "false"},
            })->WithChildren({
                em->MakeEntity<WG_Rect2d>("box_2a")->SetProps({
                    {"position", "10, 40"},
                    {"rect-dimensions", "70, 30"},
                    {"rect-color", "blue"},
                    {"clip-content", "true"},
                }),
                em->MakeEntity<WG_Rect2d>("box_2b")->SetProps({
                    {"position", "10, -20"},
                    {"rect-dimensions", "70, 50"},
                    {"rect-color", "purple"},
                    {"clip-content", "true"},
                })
            }),
            em->MakeEntity<WG_Rect2d>("box_1b")->SetProps({
                {"position", "110, 10"},
                {"rect-dimensions", "100, 100"},
                {"rect-color", "green"},
                {"clip-content", "true"},
            })
            });


    em->MakeChild<WG_Rect2d>("box_0")->SetProps({
        {"position", "500, 100"},
        {"rect-dimensions", "200, 100"},
        {"rect-color", "black"},
        {"clip-content", "false"},
        })->WithChildren({
            em->MakeEntity<WG_Rect2d>("box_1a")->SetProps({
                {"position", "10, 10"},
                {"rect-dimensions", "90, 80"},
                {"rect-color", "red"},
                {"clip-content", "true"},
            })->WithChildren({
                em->MakeEntity<WG_Rect2d>("box_2a")->SetProps({
                    {"position", "10, 40"},
                    {"rect-dimensions", "70, 30"},
                    {"rect-color", "blue"},
                    {"clip-content", "true"},
                }),
                em->MakeEntity<WG_Rect2d>("box_2b")->SetProps({
                    {"position", "10, -20"},
                    {"rect-dimensions", "70, 50"},
                    {"rect-color", "purple"},
                    {"clip-content", "true"},
                })
            }),
            em->MakeEntity<WG_Rect2d>("box_1b")->SetProps({
                {"position", "110, 10"},
                {"rect-dimensions", "100, 100"},
                {"rect-color", "green"},
                {"clip-content", "true"},
            })
            });


    em->MakeChild<WG_Rect2d>("box_0")->SetProps({
        {"position", "500, 400"},
        {"rect-dimensions", "200, 100"},
        {"rect-color", "black"},
        {"clip-content", "false"},
        })->WithChildren({
            em->MakeEntity<WG_Rect2d>("box_1a")->SetProps({
                {"position", "10, 10"},
                {"rect-dimensions", "90, 80"},
                {"rect-color", "red"},
                {"clip-content", "false"},
            })->WithChildren({
                em->MakeEntity<WG_Rect2d>("box_2a")->SetProps({
                    {"position", "10, 40"},
                    {"rect-dimensions", "70, 30"},
                    {"rect-color", "blue"},
                    {"clip-content", "false"},
                }),
                em->MakeEntity<WG_Rect2d>("box_2b")->SetProps({
                    {"position", "10, -20"},
                    {"rect-dimensions", "70, 50"},
                    {"rect-color", "purple"},
                    {"clip-content", "false"},
                })
            }),
            em->MakeEntity<WG_Rect2d>("box_1b")->SetProps({
                {"position", "110, 10"},
                {"rect-dimensions", "100, 100"},
                {"rect-color", "green"},
                {"clip-content", "false"},
            })
            });
}

struct BounceBox : public virtual WG_Component, public WG_Tickable {
    glm::vec2 velo = { 0.25f, 0.25f };
    void SetProps(WG_PropsMap props) override
    {
        velo = props.ParseVec2("bounce-velo");
    }
    void Tick(float delta) override {
        auto Transform = EnsureComponent<WG_Transform>();
        if (Transform->Position.x + 100 > 800 || Transform->Position.x < 0) velo.x = -velo.x;
        if (Transform->Position.y + 100 > 600 || Transform->Position.y < 0) velo.y = -velo.y;
        // Transform->Position += glm::vec3{velo.x * delta, velo.y * delta, 0.0f};
    }
};

void drawingSamples(std::shared_ptr<WG_Engine> engine)
{

    // TopBox (White)
    //      BottomBox (Black)
    //        White should draw and black should draw above it, but clipped to white's bounds

    std::shared_ptr<WG_EntityManager> em = engine->GetEntityManager();

    em->MakeChild<WG_Rect2d, BounceBox>("top-box")->SetProps({
        {"position", "100, 200"},
        {"rect-color", "white"},
        { "bounce-velo", "0.5, 0.1" }
        })->WithChildren({
            em->MakeEntity<WG_Rect2d, BounceBox>("bottom-box")->SetProps({
                {"position", "10, 10"},
                {"rect-color", "black"},
            })
    });
}

void styleSamples(std::shared_ptr<WG_Engine> engine)
{
    std::shared_ptr<WG_EntityManager> em = engine->GetEntityManager();

    auto c = em->MakeChild<WG_Rect2d, BounceBox>("border-testing-1")->SetProps({
        {"position", "100, 200"},
        {"rect-color", "255, 255, 255, 100"},
        {"border-thickness", "10.0"},
        {"border-color", "black"},

        { "bounce-velo", "0.5, 0.1" }
    });

    auto trans = c->EnsureComponent<WG_Transform>();
    trans->Rotation = glm::rotate(trans->Rotation, 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));

    ANONCOMPD(c, public WG_Tickable {
        bool rising_thickness = false;
        void Tick(float delta) override {
            auto ui = EnsureComponent<WG_UiRenderable>();
            if (ui->BorderThickness <= 10) rising_thickness = true;
            if (ui->BorderThickness > 100) rising_thickness = false;
            ui->BorderThickness = ui->BorderThickness + (rising_thickness ? (delta * 0.1f) : (delta * -0.1f));
        }
    });
}


void sample2d(std::shared_ptr<WG_Engine> engine)
{
    styleSamples(engine);
}
