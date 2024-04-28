#include <SFML/Graphics.hpp>
#include "main.h"
#include <string.h>
#include <thread>
#include <cstdio>
// https://pixspy.com/

Ground *ground_array;
int ground_array_size = 16;
// realocate the ground array and add new ground

void add_ground(sf::Texture &ground_texture, int x, int y)
{
    ground_array = (Ground *)realloc(ground_array, sizeof(Ground) * (ground_array_size + 1));
    Ground ground(x, y, &ground_texture);
    ground.set_position(x, y);
    memcpy(&ground_array[ground_array_size], &ground, sizeof(Ground));
    ground_array_size++;
}

void create_ground(sf::Texture &ground_texture)
{
    for (int i = 0; i < 16; i++)
    {
        Ground ground(i * 64, 800 - 64, &ground_texture);
        ground.set_position(i * 64, 800 - 64);
        memcpy(&ground_array[i], &ground, sizeof(Ground));
    }
}

bool check_Y_collision(float x, float y, int width, int height)
{
    // check Y
    for (int i = 0; i < ground_array_size; i++)
    {
        if (sf::FloatRect(x, y, width, height).intersects(ground_array[i].sprite.getGlobalBounds()))
        {
            return true;
        }
    }
    return false;
}

bool check_X_collision(float x, float y, int width, int height)
{
    // check X
    for (int i = 0; i < ground_array_size; i++)
    {
        if (sf::FloatRect(x, y, width, height).intersects(ground_array[i].sprite.getGlobalBounds()))
        {
            return true;
        }
    }
    return false;
}

void jump(Player &player)
{
    // create a thread to make the player jump
    std::thread t1([&player]()
                   {
                    sf::Clock clock;
                    clock.restart();

                    while(clock.getElapsedTime().asSeconds() < 0.35)
                    {
                        if(check_Y_collision(player.x, player.y-2.5, player.width, player.height) == true)
                        {
                            break;
                        }
                        player.y -= 2.5;
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    } 
                    return; });
    t1.detach();
    return;
}

void player_move(Player &player, float dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        if (check_X_collision(player.x - (100 * dt * 2), player.y, player.width, player.height - 5) == false)
        {
            player.x -= 100 * dt;
            player.moving = 1;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        if (check_X_collision(player.x + (100 * dt * 2), player.y, player.width, player.height - 5) == false)
        {
            player.x += 100 * dt;
            player.moving = 2;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        if (player.jumping == 0)
        {
            player.jumping = 1;
            jump(player);
        }
    }

    if (check_Y_collision(player.x, player.y, player.width, player.height) == false)
    {
        player.y += 100 * dt;
        player.jumping = 1;
    }
    else
    {
        player.jumping = 0;
    }
}

void enemy_move(Enemy &enemy, float dt)
{
    if (check_Y_collision(enemy.x - 40 + enemy.offset, enemy.y, enemy.width, enemy.height) == false)
    {
        enemy.y += 100 * dt;
    }
    else
    {
        if (enemy.left == 0)
        {
            enemy.x += 50 * dt;
            enemy.walk_left_right += 50 * dt;
            if (enemy.walk_left_right >= 200)
            {
                enemy.left = 1;
                enemy.walk_left_right = 0;
                enemy.offset = 40;
            }
        }
        else
        {
            enemy.x -= 50 * dt;
            enemy.walk_left_right += 50 * dt;
            if (enemy.walk_left_right >= 200)
            {
                enemy.left = 0;
                enemy.walk_left_right = 0;
                enemy.offset = 0;
            }
        }
    }
}
void change_sprite_player(Player &player)
{
    if (player.moving == 0)
    {
        player.sprite.setTexture(player.player_texture_idle);

        player.animation_frame++;
        if (player.animation_frame >= 10)
            player.animation_frame = 0;
        player.sprite.setTextureRect(sf::IntRect(24 * (player.animation_frame), 0, 24, 32));
    }
    else
    {
        player.sprite.setTexture(player.player_texture_walking);
        player.animation_frame++;
        if (player.animation_frame >= 11)
            player.animation_frame = 0;

        if (player.moving == 1)
        {
            player.sprite.setScale(-3, 3);
            player.left = 1;
        }
        else
        {
            player.sprite.setScale(3, 3);
            player.left = 0;
        }

        player.sprite.setTextureRect(sf::IntRect(22 * (player.animation_frame), 0, 22, 32));
    }
}

void change_sprite_enemy(Enemy &enemy)
{
    enemy.animation_frame++;
    if (enemy.animation_frame >= 6)
        enemy.animation_frame = 0;

    if (enemy.left == 0)
    {
        enemy.sprite.setScale(-4, 4);
    }
    else
    {
        enemy.sprite.setScale(4, 4);
    }
    enemy.sprite.setTextureRect(sf::IntRect(9 + (32 * enemy.animation_frame), 56, 18, 8));
}

Player start_player()
{
    sf::Texture player_texture_walking;
    player_texture_walking.loadFromFile("assets/Skeleton Walk.png");
    sf::Texture player_texture_idle;
    player_texture_idle.loadFromFile("assets/Skeleton Idle.png");

    Player player(&player_texture_idle, &player_texture_walking);
    return player;
}

Enemy start_enemy(float x, float y)
{

    sf::Texture enemy_texture;
    enemy_texture.loadFromFile("assets/aranha.png");
    Enemy enemy(&enemy_texture, x, y);

    return enemy;
}

bool check_enemy_player_collision(Player &player, Enemy &enemy)
{
    if (sf::FloatRect(player.x, player.y, player.width, player.height).intersects(sf::FloatRect(enemy.x, enemy.y, 1, enemy.height)))
    {
        return true;
    }
    return false;
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(1024, 800),
        "Nome do Jogo");
    window.setFramerateLimit(60);

    float time_to_live_player = 90;

    sf::CircleShape circle_player(4.f);

    Player player = start_player();

    Enemy enemy = start_enemy(200, 200);

    sf::Texture ground_texture;
    ground_texture.loadFromFile("assets/mainlev_build.png");
    ground_array = (Ground *)malloc(sizeof(Ground) * 16);
    create_ground(ground_texture);

    add_ground(ground_texture, 512, 800 - 64 - 64);
    add_ground(ground_texture, 512, 800 - 64 - 64 - 64);
    // add more to make the map
    add_ground(ground_texture, 512 + 64, 800 - 64 - 64);
    add_ground(ground_texture, 512 + 64, 800 - 64 - 64 - 64);
    add_ground(ground_texture, 512 + 64 + 64, 800 - 64 - 64);
    add_ground(ground_texture, 512 + 64 + 64, 800 - 64 - 64 - 64);
    add_ground(ground_texture, 512 + 64 + 64 + 64, 800 - 64 - 64);
    add_ground(ground_texture, 512 + 64 + 64 + 64, 800 - 64 - 64 - 64 - 64 - 64);

    sf::Font font;
    font.loadFromFile("assets/Arial.ttf");
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Red);
    text.setPosition(0, 0);

    sf::Clock clock;
    clock.restart();
    sf::Clock delta_clock;
    sf::Clock time_life;

    time_life.restart();
    while (window.isOpen())
    {
        float dt = delta_clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        player.moving = 0;

        player_move(player, dt);
        enemy_move(enemy, dt);

        // every 800mills change the sprite
        if (clock.getElapsedTime().asMilliseconds() > 100)
        {
            clock.restart();
            change_sprite_player(player);
            change_sprite_enemy(enemy);
        }

        // move every sprite
        //  make the sprite follow the player face left or not
        if (player.left == 0)
        {
            player.sprite.setPosition(player.x, player.y);
        }
        else
        {
            player.sprite.setPosition(player.x + player.width, player.y);
        }

        if (enemy.left == 0)
        {
            enemy.sprite.setPosition(enemy.x, enemy.y);
        }
        else
        {
            enemy.sprite.setPosition(enemy.x - enemy.offset, enemy.y);
        }

        if (check_enemy_player_collision(player, enemy) == true)
        {
            player.x = 0;
            player.y = 0;
        }

        circle_player.setPosition(player.x, player.y); // para remover

        // write time life on the screen

        std::string timeString = std::to_string(time_to_live_player - time_life.getElapsedTime().asSeconds());
        size_t dotPos = timeString.find('.');
        if (dotPos != std::string::npos && timeString.length() - dotPos > 2)
        {
            timeString = timeString.substr(0, dotPos + 3);
        }
        text.setString("Time: " + timeString);

        window.clear();
        for (int i = 0; i < ground_array_size; i++)
        {
            window.draw(ground_array[i].sprite);
        }
        window.draw(circle_player);
        window.draw(player.sprite);
        window.draw(enemy.sprite);
        window.draw(text);
        window.display();
    }
    return 0;
}
