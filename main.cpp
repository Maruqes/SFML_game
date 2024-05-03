#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "main.h"
#include <string.h>
#include <thread>
#include <cstdio>
#include <iostream>
// https://pixspy.com/

Ground *ground_array;
Enemy *enemy_array;
Star *star_array;
int ground_array_size = 0;
int enemy_count = 0;
int star_count = 0;
int current_level = 1;
// realocate the ground array and add new ground

void add_ground(sf::Texture &ground_texture, int x, int y)
{
    ground_array = (Ground *)realloc(ground_array, sizeof(Ground) * (ground_array_size + 1));
    Ground ground(x, y, &ground_texture);
    ground.set_position(x, y);
    memcpy(&ground_array[ground_array_size], &ground, sizeof(Ground));
    ground_array_size++;
}

void add_enemy(sf::Texture &enemy_texture, int x, int y, int ani, int walk)
{
    enemy_array = (Enemy *)realloc(enemy_array, sizeof(Enemy) * (enemy_count + 1));
    Enemy enemy(&enemy_texture, x, y);
    enemy.animation_frame = ani;
    enemy.walk_left_right = walk;
    memcpy(&enemy_array[enemy_count], &enemy, sizeof(Enemy));
    enemy_count++;
}

void add_star(sf::Texture &star_texture, int x, int y)
{
    star_array = (Star *)realloc(star_array, sizeof(Star) * (star_count + 1));
    Star star(x, y, &star_texture);
    star.set_position(x, y);
    memcpy(&star_array[star_count], &star, sizeof(Star));
    star_count++;
}

bool check_collision(float x, float y, int width, int height)
{
    for (int i = 0; i < ground_array_size; i++)
    {
        if (sf::FloatRect(x, y, width, height).intersects(ground_array[i].sprite.getGlobalBounds()))
        {
            return true;
        }
    }
    return false;
}

bool check_enemy_player_collision(Player &player, Enemy &enemy)
{
    if (sf::FloatRect(player.x, player.y, player.width, player.height).intersects(sf::FloatRect(enemy.x, enemy.y, 1, enemy.height)))
    {
        if (enemy.dead == true)
        {
            return false;
        }
        return true;
    }
    return false;
}

bool check_colistion_player_star(Player &player, Star &star)
{
    if (sf::FloatRect(player.x, player.y, player.width, player.height).intersects(star.sprite.getGlobalBounds()))
    {
        return true;
    }
    return false;
}

bool check_atack_enemies(Player &player, Enemy &enemy)
{
    if (enemy.dead == true)
        return false;

    if (player.left == 0)
    {
        if (sf::FloatRect(player.x + player.width, player.y, player.width, player.height).intersects(sf::FloatRect(enemy.x, enemy.y, enemy.width, enemy.height)))
        {
            enemy.dead = true;
            return true;
        }
    }
    else
    {
        if (sf::FloatRect(player.x, player.y, player.width, player.height).intersects(sf::FloatRect(enemy.x, enemy.y, enemy.width, enemy.height)))
        {
            enemy.dead = true;
            return true;
        }
    }

    return false;
}

void jump(Player &player, float dt)
{
    // create a thread to make the player jump
    std::thread t1([&player, dt]()
                   {
            sf::Clock clock;
            clock.restart();
            while (clock.getElapsedTime().asSeconds() < 0.35)
            {
                    if (check_collision(player.x, player.y - 500 * dt, player.width , player.height) == true)
                    {
                        printf("Colidiu\n");
                        break;
                    }

                player.y -= 500 * dt;
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            return; });
    t1.detach();
    return;
}

void change_sprite_player(Player &player)
{
    if (player.atacking == 1)
    {
        player.sprite.setTexture(player.player_texture_atack);
        player.animation_frame++;
        if (player.animation_frame >= 18)
            player.animation_frame = 0;
        player.sprite.setTextureRect(sf::IntRect(43 * (player.animation_frame), 0, 43, 37));
        return;
    }
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
            if (check_collision(player.x, player.y, -24 * 3, player.height - 4) == false)
            {
                player.sprite.setScale(-3, 3);
                player.left = 1;
                player.width = -24 * 3;
            }
        }
        else
        {
            if (check_collision(player.x, player.y, 24 * 3, player.height - 4) == false)
            {
                player.sprite.setScale(3, 3);
                player.left = 0;
                player.width = 24 * 3;
            }
        }

        player.sprite.setTextureRect(sf::IntRect(22 * (player.animation_frame), 0, 22, 32));
    }
}

void atack(Player &player)
{
    if (player.atacking == 1)
        return;

    std::thread t1([&player]()
                   {
            sf::Clock clock;
            sf::Clock animation_clock;
            bool animatee=true;
            clock.restart();
            player.atacking = 1;


            printf("Atack\n");
            for (int i = 0; i < enemy_count; i++)
            {
                if (check_atack_enemies(player, enemy_array[i]) == true)
                {
                    break;
                }
            }
            player.animation_frame = 0;
            while(animatee)
            {
                change_sprite_player(player);
                if (animation_clock.getElapsedTime().asSeconds() > 0.7)
                {
                    animatee = false;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            while (clock.getElapsedTime().asSeconds() < 0.3)
            {
                player.time_to_atack_again = clock.getElapsedTime().asSeconds();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            player.atacking = 0;
            return; });

    t1.detach();
    return;
}

void player_move(Player &player, float dt)
{

    if (!player.atacking)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            if (check_collision(player.x - ((100 * dt * 2) * 5), player.y, player.width, player.height - 5) == false)
            {
                player.x -= 100 * dt;
                player.moving = 1;
                player.last_direction = 1;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            if (check_collision(player.x + ((100 * dt * 2) * 2), player.y, player.width, player.height - 5) == false)
            {
                player.x += 100 * dt;
                player.moving = 2;
                player.last_direction = 2;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            if (player.atacking == 0)
            {
                atack(player);
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            if (player.jumping == 0)
            {
                player.jumping = 1;
                jump(player, dt);
            }
        }
    }

    // gravity
    if (player.left == 0)
    {
        if (check_collision(player.x, player.y + (170 * dt), player.width, player.height - 5) == false) // 5*4 = machado offset
        {
            player.y += 170 * dt;
            player.jumping = 1;
        }
        else
        {
            player.jumping = 0;
        }
    }
    else
    {
        if (check_collision(player.x, player.y + (170 * dt), player.width, player.height - 5) == false) // 5*4 = machado offset
        {
            player.y += 170 * dt;
            player.jumping = 1;
        }
        else
        {
            player.jumping = 0;
        }
    }

    if (player.y > 800)
    {
        player.x = 0;
        player.y = 0;
    }
}

void enemy_move(Enemy &enemy, float dt)
{
    if (check_collision(enemy.x - 40 + enemy.offset, enemy.y, enemy.width, enemy.height) == false)
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

void change_sprite_star(Star &star)
{
    star.animation_frame++;
    if (star.animation_frame >= 26) // 13*2
        star.animation_frame = 0;

    star.sprite.setTextureRect(sf::IntRect(0 + (32 * (star.animation_frame / 2)), 0, 32, 32));

    // nesta funcao tenho 13 frames, multiplico por 2 para ter 26 frames e ficar a animacoa mais lenta TER EM ATENCAO AO star.animation_frame / 2
}

Player start_player()
{
    sf::Texture player_texture_walking;
    player_texture_walking.loadFromFile("assets/Skeleton Walk.png");
    sf::Texture player_texture_idle;
    player_texture_idle.loadFromFile("assets/Skeleton Idle.png");
    sf::Texture player_texture_atackk;
    player_texture_atackk.loadFromFile("assets/Skeleton Attack.png");

    Player player(&player_texture_idle, &player_texture_walking, &player_texture_atackk, 0, 0);
    return player;
}

Enemy start_enemy(float x, float y)
{

    sf::Texture enemy_texture;
    enemy_texture.loadFromFile("assets/aranha.png");
    Enemy enemy(&enemy_texture, x, y);

    return enemy;
}

void level_load1(Player &player, sf::Texture &ground_texture, sf::Texture &enemy_texture, sf::Texture &star_texture)
{
    int starting_pos[2] = {0, 0};
    player.x = starting_pos[0];
    player.y = starting_pos[1];

    for (int i = 0; i < 22; i++)
    {
        add_ground(ground_texture, i * 48, 800 - 48);
    }
    // primeiroandar
    add_ground(ground_texture, 0, 96);
    add_ground(ground_texture, 48, 96);
    add_ground(ground_texture, 96, 96);
    add_ground(ground_texture, 144, 96);
    add_ground(ground_texture, 192, 96);
    add_ground(ground_texture, 240, 96);
    add_ground(ground_texture, 288, 96);
    add_ground(ground_texture, 336, 96);
    add_ground(ground_texture, 384, 96);
    add_ground(ground_texture, 432, 96);
    add_ground(ground_texture, 480, 96);
    add_ground(ground_texture, 528, 96);
    add_ground(ground_texture, 576, 96);
    add_ground(ground_texture, 624, 96);
    add_ground(ground_texture, 672, 96);
    add_ground(ground_texture, 720, 96);
    add_ground(ground_texture, 768, 96);
    add_ground(ground_texture, 816, 96);
    add_ground(ground_texture, 864, 96);
    //
    // segundo andar

    add_ground(ground_texture, 144, 240);
    add_ground(ground_texture, 192, 240);
    add_ground(ground_texture, 240, 240);
    add_ground(ground_texture, 288, 240);
    add_ground(ground_texture, 336, 240);
    add_ground(ground_texture, 384, 240);
    add_ground(ground_texture, 432, 240);
    add_ground(ground_texture, 480, 240);
    add_ground(ground_texture, 528, 240);
    add_ground(ground_texture, 576, 240);
    add_ground(ground_texture, 624, 240);
    add_ground(ground_texture, 672, 240);
    add_ground(ground_texture, 720, 240);
    add_ground(ground_texture, 768, 240);
    add_ground(ground_texture, 816, 240);
    add_ground(ground_texture, 864, 240);
    add_ground(ground_texture, 960, 240);
    add_ground(ground_texture, 912, 240);
    add_ground(ground_texture, 1008, 240);
    //
    add_ground(ground_texture, 0, 384);
    add_ground(ground_texture, 48, 384);
    add_ground(ground_texture, 192, 528);
    add_ground(ground_texture, 144, 528);
    add_ground(ground_texture, 0, 672);
    add_ground(ground_texture, 48, 672);

    // coluna
    add_ground(ground_texture, 240, 288);
    add_ground(ground_texture, 240, 384);
    add_ground(ground_texture, 240, 528);
    add_ground(ground_texture, 240, 528);

    add_ground(ground_texture, 240, 288);
    add_ground(ground_texture, 240, 336);
    add_ground(ground_texture, 240, 384);
    add_ground(ground_texture, 240, 432);
    add_ground(ground_texture, 240, 480);
    add_ground(ground_texture, 240, 528);
    //

    add_ground(ground_texture, 288, 528);
    add_ground(ground_texture, 336, 528);
    add_ground(ground_texture, 384, 528);
    add_ground(ground_texture, 432, 528);
    add_ground(ground_texture, 480, 528);
    add_ground(ground_texture, 528, 528);
    add_ground(ground_texture, 576, 528);
    add_ground(ground_texture, 624, 528);
    add_ground(ground_texture, 672, 528);
    add_ground(ground_texture, 720, 528);
    add_ground(ground_texture, 768, 528);
    //
    add_ground(ground_texture, 1008, 624);
    add_ground(ground_texture, 960, 624);
    add_ground(ground_texture, 912, 624);

    add_ground(ground_texture, 768, 384);
    add_ground(ground_texture, 768, 438);
    add_ground(ground_texture, 768, 480);

    add_ground(ground_texture, 720, 384);
    add_ground(ground_texture, 672, 384);
    add_ground(ground_texture, 576, 384);
    add_ground(ground_texture, 528, 384);
    add_ground(ground_texture, 480, 384);
    add_ground(ground_texture, 432, 384);
    add_ground(ground_texture, 384, 384);
    add_ground(ground_texture, 624, 384);

    // add more to make the map

    // teto

    add_enemy(enemy_texture, 200, 200, 0, 0);
    add_enemy(enemy_texture, 100, 200, 5, 30);

    add_star(star_texture, 640, 512);
    add_star(star_texture, 832, 670);
    current_level = 1;
}

void level_load2(Player &player, sf::Texture &ground_texture, sf::Texture &enemy_texture, sf::Texture &star_texture)
{

    int starting_pos[2] = {0, 0};
    player.x = starting_pos[0];
    player.y = starting_pos[1];

    add_ground(ground_texture, 0, 192);
    add_ground(ground_texture, 64, 192);
    add_ground(ground_texture, 128, 192);
    add_ground(ground_texture, 64 * 4, 256);
    add_ground(ground_texture, 64 * 5, 256);
    add_ground(ground_texture, 64 * 6, 256);
    add_ground(ground_texture, 64 * 7, 256);
    add_ground(ground_texture, 64 * 8, 256);
    add_ground(ground_texture, 64 * 9, 256);
    add_ground(ground_texture, 64 * 10, 256);

    add_enemy(enemy_texture, 448, 170, 0, 0);

    add_star(star_texture, 320, 192);
    add_star(star_texture, 640, 192);
    current_level = 2;
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(1024, 800),
        "Nome do Jogo");
    window.setFramerateLimit(60);

    float time_to_live_player = 90;
    int stars_found = 0;

    sf::CircleShape circle_player(4.f);

    Player player = start_player();

    sf::Texture enemy_texture;
    sf::Texture ground_texture;
    sf::Texture star_texture;

    enemy_texture.loadFromFile("assets/aranha.png");
    ground_texture.loadFromFile("assets/mainlev_build.png");
    star_texture.loadFromFile("assets/Star.png");

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

    level_load1(player, ground_texture, enemy_texture, star_texture);
    while (window.isOpen())
    {
        float dt = delta_clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        player.moving = 0;

        // mover tudo
        player_move(player, dt);
        for (int i = 0; i < enemy_count; i++)
        {
            enemy_move(enemy_array[i], dt);
        }

        // every 800mills change the sprite
        if (clock.getElapsedTime().asMilliseconds() > 100)
        {
            clock.restart();
            change_sprite_player(player);
            for (int i = 0; i < enemy_count; i++)
                change_sprite_enemy(enemy_array[i]);

            for (int i = 0; i < star_count; i++)
                change_sprite_star(star_array[i]);
        }

        // check colistions
        for (int i = 0; i < enemy_count; i++)
        {
            if (check_enemy_player_collision(player, enemy_array[i]) == true) // morreuuu
            {
                player.x = 0;
                player.y = 0;
            }
        }

        for (int i = 0; i < star_count; i++)
        {
            if (check_colistion_player_star(player, star_array[i])) // check star windawin
            {
                star_array[i].set_position(-100, -100);
                stars_found++;
                if (stars_found == star_count)
                {
                    if (current_level == 1)
                    {
                        enemy_count = 0;
                        free(enemy_array);
                        enemy_array = NULL;

                        star_count = 0;
                        free(star_array);
                        star_array = NULL;

                        ground_array_size = 0;
                        free(ground_array);
                        ground_array = NULL;

                        stars_found = 0;
                        level_load2(player, ground_texture, enemy_texture, star_texture);
                        continue;
                    }

                    if (current_level == 2)
                    {
                        enemy_count = 0;
                        free(enemy_array);
                        enemy_array = NULL;

                        star_count = 0;
                        free(star_array);
                        star_array = NULL;

                        ground_array_size = 0;
                        free(ground_array);
                        ground_array = NULL;

                        stars_found = 0;
                        level_load1(player, ground_texture, enemy_texture, star_texture);
                        continue;
                    }
                }
            }
        }

        // move every sprite
        //  make the sprite follow the player face left or not
        if (player.left == 0)
        {
            if (player.atacking == 1)
            {
                player.sprite.setPosition(player.x - 20, player.y - 10);
            }
            else
            {
                player.sprite.setPosition(player.x, player.y);
            }
        }
        else
        {
            if (player.atacking == 1)
            {
                player.sprite.setPosition(player.x + 20, player.y - 10);
            }
            else
            {
                player.sprite.setPosition(player.x, player.y);
            }
        }

        for (int i = 0; i < enemy_count; i++)
        {
            if (enemy_array[i].left == 0)
            {
                enemy_array[i].sprite.setPosition(enemy_array[i].x, enemy_array[i].y);
            }
            else
            {
                enemy_array[i].sprite.setPosition(enemy_array[i].x - enemy_array[i].offset, enemy_array[i].y);
            }
        }

        for (int i = 0; i < star_count; i++)
        {
            star_array[i].sprite.setPosition(star_array[i].x, star_array[i].y);
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
        if (time_to_live_player - time_life.getElapsedTime().asSeconds() < 0)
        {
            return 0;
        }

        sf::Vector2i cursorPosition = sf::Mouse::getPosition(window);
        // printf("X: %d Y: %d\n", cursorPosition.x / 48 * 48, cursorPosition.y / 48 * 48);

        // DRAW
        window.clear();
        for (int i = 0; i < ground_array_size; i++)
        {
            window.draw(ground_array[i].sprite);
        }
        for (int i = 0; i < enemy_count; i++)
        {
            if (enemy_array[i].dead == true)
                continue;
            window.draw(enemy_array[i].sprite);
        }
        for (int i = 0; i < star_count; i++)
        {
            window.draw(star_array[i].sprite);
        }
        window.draw(circle_player);
        window.draw(player.sprite);
        window.draw(text);
        window.display();
    }
    return 0;
}