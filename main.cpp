#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <memory>

#include "src/Pong.hpp"

/*
 * Recreation of Pong from memory
 *
 * I've no game making experience, nor much C++ experience
 *
 * 
 *
*/
int main(int argc, char** argv)
{

    Pong pong(800, 600);
    pong.Run();

}