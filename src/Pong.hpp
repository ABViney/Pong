#ifndef PONG_HPP
#define PONG_HPP

#include <SFML/Graphics.hpp>

#include <memory>
#include <vector>

class Pong;
class Scene;
class ScoreCard;
class Game;
class AABB;
class CPU;

////////////////////
// Pong
///////////////////
enum PongFlag
{
    Quit = -1
};

class Pong
{
public:
    Pong(unsigned int width, unsigned int height);
    ~Pong() = default;

    void Run();
    void SetFlag(PongFlag flag);

private:
    unsigned int m_Width, m_Height;
    std::shared_ptr<sf::RenderWindow> m_Window;
    std::shared_ptr<Scene> m_Current;

    bool shouldQuit;
};

////////////////////
// Scene
///////////////////
class Scene
{
public:
    Scene(Pong& container)
        : m_Container(&container)
    {}
    virtual ~Scene() {};

    virtual void Tick() = 0;
    virtual void ReadEvent(sf::Event& event) = 0;
    void Draw(sf::RenderWindow& window);

protected:
    std::shared_ptr<Pong> m_Container;
    std::vector<std::shared_ptr<sf::RectangleShape>> m_Rects;
};

//////////////////
// ScoreCard
/////////////////
class ScoreCard
{
public:
    ScoreCard();
    ~ScoreCard() = default;

    void SetPosition(float x, float y);
    void Incr();
    inline int GetScore() const { return m_Score; }

    // std::vector<std::shared_ptr<sf::RectangleShape>> GetShapes();
    std::vector<sf::RectangleShape*> GetShapes();

private:
    inline void Off(sf::RectangleShape& r) { r.setFillColor(sf::Color::Transparent); }
    inline void On (sf::RectangleShape& r) { r.setFillColor(sf::Color::White); }

    int m_Score;
    sf::RectangleShape r_a1, r_b1, r_c1, r_d1, r_e1, r_f1, r_g1,
                       r_a2, r_b2, r_c2, r_d2, r_e2, r_f2, r_g2;
};

//////////////
// Game Process
//////////////
class Game : public Scene
{
public:
    Game(Pong& container);
    ~Game() = default;

    void Setup();
    void Tick();
    void ReadEvent(sf::Event& event);

private:
    sf::RectangleShape m_Paddle1, m_Paddle2, m_Ball, m_TopBound, m_BotBound;
    ScoreCard m_Player_L, m_Player_R;

    bool roundStart;
    float m_BallX = 0,
          m_BallY = 0,
          m_BallLastX = 0,
          m_BallLastY = 0,
          m_BallVelX = 0,
          m_BallVelY = 0;

    const float p_MoveSpeed = 0.1f;
    const float b_startX = 395.f; const float b_startY = 345.f;
    
    bool p_1MU, p_1MD, p_2MU, p_2MD, p2_Present;

    void BallLaunch();
    void BallLogic();
};

/////////////////
// Collision Checking
////////////////
class AABB
{
public:
    static inline bool CheckCollision(sf::RectangleShape r1, sf::RectangleShape r2)
    {
        if((r1.getPosition().x < r2.getPosition().x+r2.getLocalBounds().width) &&
           (r1.getPosition().x+r1.getLocalBounds().width > r2.getPosition().x) &&
           (r1.getPosition().y < r2.getPosition().y+r2.getLocalBounds().height)&&
           (r1.getPosition().y+r1.getLocalBounds().height> r2.getPosition().y))
           return true;
        return false;
    }
};

class CPU
{
public:
    static void Think(sf::RectangleShape& paddle, sf::RectangleShape& ball, bool* p_2MU, bool* p_2MD)
    {
        if(ball.getPosition().y+ball.getLocalBounds().height < paddle.getPosition().y) // If ball lower left higher than paddle top left
            *p_2MU = true;
        else *p_2MU = false;
        if(ball.getPosition().y > paddle.getPosition().y+paddle.getLocalBounds().height) // If ball top left lower than paddle bottom left
            *p_2MD = true;
        else *p_2MD = false;
    }

};

#endif /* Pong.hpp */