#include "Pong.hpp"

#include <cstdlib>
#include <iostream>

//////////
// Pong
//////////
Pong::Pong(unsigned int width, unsigned int height)
    : m_Width(width), m_Height(height),
      m_Window(new sf::RenderWindow(sf::VideoMode(width, height), "Pong"))
{
    m_Current = std::make_shared<Game>(*this);
}

void Pong::Run()
{
    while(!shouldQuit && m_Window->isOpen())
    {
        sf::Event event;
        while(m_Window->pollEvent(event))
        {
            if(event.type == sf::Event::Closed) // Temp 
            {
                shouldQuit = true;
                break;
            }
            m_Current->ReadEvent(event);
        }

        m_Current->Tick();
        m_Current->Draw(*m_Window);
    }
}

void Pong::SetFlag(PongFlag flag)
{
    switch(flag)
    {
    case -1:
        shouldQuit = true;
        break;
    }
}

/////////
// Scene
//////////
void Scene::Draw(sf::RenderWindow& window)
{
    window.clear();
    for(std::shared_ptr<sf::RectangleShape> rect : m_Rects) window.draw(*rect);
    window.display();
}

/////////
// Game
///////////
Game::Game(Pong& container)
    : Scene(container)
{
    m_Paddle1.setSize(sf::Vector2f(10.f, 50.f));
    m_Paddle1.setPosition(50.f, 110.f);
    m_Paddle1.setFillColor(sf::Color::White);

    m_Paddle2.setSize(sf::Vector2f(10.f, 50.f));
    m_Paddle2.setPosition(740.f, 450.f);
    m_Paddle2.setFillColor(sf::Color::White);

    m_Ball.setSize(sf::Vector2f(10.f, 10.f));
    m_Ball.setPosition(395.f, 345.f);
    m_Ball.setFillColor(sf::Color::White);

    m_TopBound.setSize(sf::Vector2f(800.f, 10.f));
    m_TopBound.setPosition(sf::Vector2f(0.f, 90.f));
    m_TopBound.setFillColor(sf::Color::White);

    m_BotBound.setSize(sf::Vector2f(800.f, 10.f));
    m_BotBound.setPosition(sf::Vector2f(0.f, 600.f));
    m_BotBound.setFillColor(sf::Color::White);

    this->m_Rects.emplace_back(&m_Paddle1);
    this->m_Rects.emplace_back(&m_Paddle2);
    this->m_Rects.emplace_back(&m_Ball);
    this->m_Rects.emplace_back(&m_TopBound);
    this->m_Rects.emplace_back(&m_BotBound);

}

void Game::Tick() // Todo - Remove magic numbers from collision checks
{
    m_Ball.setPosition(m_BallX, m_BallY);
    BallLogic();

    // Player 1 Movement
    if(p_1MU) 
    {
        m_Paddle1.setPosition(m_Paddle1.getPosition().x, m_Paddle1.getPosition().y - p_MoveSpeed);
        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) p_1MU = false;
    }
    if(p_1MD) 
    {
        m_Paddle1.setPosition(m_Paddle1.getPosition().x, m_Paddle1.getPosition().y + p_MoveSpeed);
        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) p_1MD = false;
    }
    
    if(AABB::CheckCollision(m_Paddle1, m_TopBound))
    {
        m_Paddle1.setPosition(m_Paddle1.getPosition().x, 100.f);
    }
    else if(AABB::CheckCollision(m_Paddle1, m_BotBound))
    {
        m_Paddle1.setPosition(m_Paddle1.getPosition().x, 550.f);
    }

    // Player 2 -- CPU Movement
    CPU::Think(m_Paddle2, m_Ball, &p_2MU, &p_2MD); // Put check for other player

    if(p_2MU) 
    {
        m_Paddle2.setPosition(m_Paddle2.getPosition().x, m_Paddle2.getPosition().y - p_MoveSpeed);
        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) p_2MU = false;
    }
    if(p_2MD) 
    {
        m_Paddle2.setPosition(m_Paddle2.getPosition().x, m_Paddle2.getPosition().y + p_MoveSpeed);
        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) p_2MD = false;
    }

    if(AABB::CheckCollision(m_Paddle2, m_TopBound))
    {
        m_Paddle2.setPosition(m_Paddle2.getPosition().x, 100.f);
    }
    else if(AABB::CheckCollision(m_Paddle2, m_BotBound))
    {
        m_Paddle2.setPosition(m_Paddle2.getPosition().x, 550.f);
    }
    
}

void Game::BallLaunch()
{
    std::cout << " Round Start " << std::endl;
    roundStart = true;
    // Put point tracking to launch toward whoever scored last
    m_BallX = b_startX; m_BallY = b_startY;

    m_BallVelX = -0.02f;
    m_BallVelY = -0.01f;
}

void Game::BallLogic()
{
    if(roundStart)
    {
        m_BallLastX = m_BallX;
        m_BallLastY = m_BallY;

        m_BallX += m_BallVelX;
        m_BallY += m_BallVelY;

        if(AABB::CheckCollision(m_TopBound, m_Ball))
        {
            m_BallY = 100.f;
            m_BallVelY *= -1;
        }
        else if(AABB::CheckCollision(m_BotBound, m_Ball))
        {
            m_BallY = 600.f - m_Ball.getLocalBounds().height;
            m_BallVelY *= -1;
        }

        // Todo -- Make Bloops
        if(AABB::CheckCollision(m_Paddle1, m_Ball))
        {
            m_BallX = m_Paddle1.getPosition().x + m_Paddle1.getLocalBounds().width;
            m_BallVelX *= -1;
            m_BallVelX += m_BallVelX/2;
            m_BallVelY += m_BallVelY/2;
        }
        else if(AABB::CheckCollision(m_Paddle2, m_Ball))
        {
            m_BallX = m_Paddle2.getPosition().x - m_Ball.getLocalBounds().width;
            m_BallVelX *= -1;
            m_BallVelX += m_BallVelX/2;
            m_BallVelY += m_BallVelY/2;
        }

        if(m_Ball.getPosition().x+m_Ball.getLocalBounds().width < 0 || m_Ball.getPosition().x > 800.f)
        {
            // Assign point, restart round
            roundStart = false;
            m_BallX = b_startX;
            m_BallY = b_startY;
        }


    }
}

void Game::ReadEvent(sf::Event& event)
{
    if(event.type == sf::Event::KeyPressed)
    {
        if(!roundStart) BallLaunch();
        if(event.key.code == sf::Keyboard::Escape)
        {
            this->m_Container->SetFlag(PongFlag::Quit);
        }
        if(event.key.code == sf::Keyboard::Up)
        {
           p_1MU = true;
        }
        if(event.key.code == sf::Keyboard::Down)
        {
            p_1MD = true;
        }
    }
}

///////////////////////////////////////////////////////
//     a      /  
//    ---     / 7-Segment digital display
// f |   | b  / 
//   |   |    /
//    ---     /
// e | g | c  /
//   |   |    /
//    ---     /
//     d      /
// 
// ScoreCard::ScoreCard(float originX, float originY)
//     : m_Score(0)
// {
//     a1 = b1 = c1 = d1 = e1 = f1 = true; // 0
//     g1 = a2 = b2 = c2 = d2 = e2 = f2 = g2 = false; // Blank

//     sf::Vector2f row_bars(25.f, 10.f);
//     sf::Vector2f col_bars(10.f, 25.f);
//     r_a1.setSize(row_bars); r_a2.setSize(row_bars); r_d1.setSize(row_bars); r_d2.setSize(row_bars); r_g1.setSize(row_bars); r_g2.setSize(row_bars);
//     r_b1.setSize(col_bars); r_c1.setSize(col_bars); r_e1.setSize(col_bars); r_f1.setSize(col_bars);
//     r_b2.setSize(col_bars); r_c2.setSize(col_bars); r_e2.setSize(col_bars); r_f2.setSize(col_bars);
//     r_a1.setFillColor(sf::Color::White); r_b1.setFillColor(sf::Color::White); r_c1.setFillColor(sf::Color::White); r_d1.setFillColor(sf::Color::White); r_e1.setFillColor(sf::Color::White); r_f1.setFillColor(sf::Color::White);
//     r_g1.setFillColor(sf::Color::White);
//     r_a1.setFillColor(sf::Color::White); r_b1.setFillColor(sf::Color::White); r_c1.setFillColor(sf::Color::White); r_d1.setFillColor(sf::Color::White); r_e1.setFillColor(sf::Color::White); r_f1.setFillColor(sf::Color::White); r_g1.setFillColor(sf::Color::White);

//     sf::Vector2f origin(originX, originY);
//     r_f2.setPosition(origin);
// }

//     ~ScoreCard() = default;

//     void Incr();

// private:
//     int m_Score;
//     bool a1,b1,c1,d1,e1,f1,g1,
//          a2,b2,d2,d2,e2,f2,g2;
//     std::vector<std::shared_ptr<sf::RectangleShape>> m_Bars;
// };