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
sf::Texture background;
sf::Sprite background_sprite;

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
                if (check_collision(player.x, player.y - 500 * dt, player.width, player.height) == true)
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
        if (player.animation_frame == 8)
        {
            for (int i = 0; i < enemy_count; i++)
            {
                if (check_atack_enemies(player, enemy_array[i]) == true)
                {
                    break;
                }
            }
        }

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
            bool animatee = true;
            clock.restart();
            player.atacking = 1;


            printf("Atack\n");

            player.animation_frame = 0;
            while (animatee)
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            if (check_collision(player.x - ((100 * dt * 2) * 5), player.y, player.width, player.height - 5) == false)
            {
                player.x -= 100 * dt;
                player.moving = 1;
                player.last_direction = 1;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            if (check_collision(player.x + ((100 * dt * 2) * 2), player.y, player.width, player.height - 5) == false)
            {
                player.x += 100 * dt;
                player.moving = 2;
                player.last_direction = 2;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            if (player.atacking == 0)
            {
                atack(player);
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
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
        if (check_collision(player.x, player.y + (170 * dt), player.width, player.height - 2) == false) // 5*4 = machado offset
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
        player.x = player.start_pos_x;
        player.y = player.start_pos_y;
        player.vidas--;
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
    int starting_pos[2] = {50, 0};
    player.x = starting_pos[0];
    player.y = starting_pos[1];
    player.start_pos_x = starting_pos[0];
    player.start_pos_y = starting_pos[1];
    background.loadFromFile("assets/background4a.png");
    background_sprite.setTexture(background);

    for (int i = 0; i < 22; i++)
    {
        add_ground(ground_texture, i * 48, 800 - 48);
    }

    for (int i = 0; i < 36; i++)
    {
        add_ground(ground_texture, i * 24, 96);
    }

    //
    for (int i = 8; i < 43; i++)
    {
        add_ground(ground_texture, i * 24, 240);
    }

    //
    add_ground(ground_texture, 0, 384);
    add_ground(ground_texture, 48, 384);
    add_ground(ground_texture, 96, 384);
    add_ground(ground_texture, 144, 528);
    add_ground(ground_texture, 192, 528);
    add_ground(ground_texture, 240, 528);
    add_ground(ground_texture, 0, 678);
    add_ground(ground_texture, 48, 678);
    add_ground(ground_texture, 96, 678);

    //
    for (int i = 10; i < 22; i++)
    {
        add_ground(ground_texture, 288, i * 24);
    }

    add_ground(ground_texture, 0, 696);
    add_ground(ground_texture, 0, 720);
    add_ground(ground_texture, 0, 744);
    add_ground(ground_texture, 24, 696);
    add_ground(ground_texture, 24, 720);
    add_ground(ground_texture, 24, 744);
    add_ground(ground_texture, 48, 696);
    add_ground(ground_texture, 48, 720);
    add_ground(ground_texture, 48, 744);
    add_ground(ground_texture, 24, 768);
    add_ground(ground_texture, 48, 768);
    add_ground(ground_texture, 0, 768);
    add_ground(ground_texture, 0, 744);
    add_ground(ground_texture, 0, 744);

    //
    add_ground(ground_texture, 96, 696);
    add_ground(ground_texture, 96, 720);
    add_ground(ground_texture, 96, 744);
    add_ground(ground_texture, 96, 768);

    ///
    for (int i = 0; i < 43; i++)
    {
        add_ground(ground_texture, i * 24, 744);
    }
    //
    for (int i = 0; i < 43; i++)
    {
        add_ground(ground_texture, i * 24, 768);
    }
    //
    for (int i = 6; i < 38; i++)
    {
        add_ground(ground_texture, i * 24, 528);
    }

    for (int i = 22; i < 32; i++)
    {
        add_ground(ground_texture, i * 24, 384);
    }

    for (int i = 0; i < 43; i++)
    {
        add_ground(ground_texture, i * 24, 768);
    }

    for (int i = 16; i < 22; i++)
    {
        add_ground(ground_texture, 744, i * 24);
    }
    add_ground(ground_texture, 912, 768);
    add_ground(ground_texture, 936, 768);
    add_ground(ground_texture, 960, 768);
    add_ground(ground_texture, 984, 768);
    add_ground(ground_texture, 1008, 768);
    add_ground(ground_texture, 912, 744);
    add_ground(ground_texture, 936, 744);
    add_ground(ground_texture, 960, 744);
    add_ground(ground_texture, 984, 744);
    add_ground(ground_texture, 1008, 744);
    add_ground(ground_texture, 984, 696);
    add_ground(ground_texture, 1008, 696);
    add_ground(ground_texture, 960, 696);
    add_ground(ground_texture, 936, 696);
    add_ground(ground_texture, 912, 696);
    add_ground(ground_texture, 984, 720);
    add_ground(ground_texture, 1008, 720);
    add_ground(ground_texture, 960, 720);
    add_ground(ground_texture, 936, 720);
    add_ground(ground_texture, 912, 720);
    add_ground(ground_texture, 984, 672);
    add_ground(ground_texture, 1008, 672);
    add_ground(ground_texture, 960, 672);
    add_ground(ground_texture, 936, 672);
    add_ground(ground_texture, 912, 672);
    add_ground(ground_texture, 888, 528);
    add_ground(ground_texture, 864, 528);
    add_ground(ground_texture, 816, 528);

    add_enemy(enemy_texture, 672, 24, 0, 0);
    add_enemy(enemy_texture, 420, 480, 4, 30);

    add_star(star_texture, 384, 30);
    add_star(star_texture, 600, 186);
    add_star(star_texture, 48, 620);
    add_star(star_texture, 864, 456);
    add_star(star_texture, 600, 412);
    add_star(star_texture, 648, 442);
    add_star(star_texture, 600, 442);
    add_star(star_texture, 648, 412);

    current_level = 1;
}

void level_load2(Player &player, sf::Texture &ground_texture, sf::Texture &enemy_texture, sf::Texture &star_texture)
{

    int starting_pos[2] = {960, 624};
    player.x = starting_pos[0];
    player.y = starting_pos[1];
    player.start_pos_x = starting_pos[0];
    player.start_pos_y = starting_pos[1];

    background.loadFromFile("assets/background3.png");
    background_sprite.setTexture(background);

    for (int i = 34; i < 43; i++)
    {
        add_ground(ground_texture, i * 24, 792);
    }
    for (int i = 34; i < 43; i++)
    {
        add_ground(ground_texture, i * 24, 768);
    }

    for (int i = 15; i < 28; i++)
    {
        add_ground(ground_texture, i * 24, 672);
    }

    for (int i = 0; i < 9; i++)
    {
        add_ground(ground_texture, i * 24, 576);
    }

    for (int i = 15; i < 28; i++)
    {
        add_ground(ground_texture, i * 24, 480);
    }
    for (int i = 33; i < 43; i++)
    {
        add_ground(ground_texture, i * 24, 360);
    }
    for (int i = 14; i < 27; i++)
    {
        add_ground(ground_texture, i * 24, 168);
    }
    for (int i = 0; i < 5; i++)
    {
        add_ground(ground_texture, i * 24, 216);
    }

    add_enemy(enemy_texture, 420, 120, 0, 30);
    add_enemy(enemy_texture, 430, 432, 2, 50);
    add_enemy(enemy_texture, 850, 312, 4, 0);
    add_star(star_texture, 456, 576);
    add_star(star_texture, 72, 480);
    add_star(star_texture, 456, 384);
    add_star(star_texture, 864, 240);
    add_star(star_texture, 432, 72);
    add_star(star_texture, 24, 120);

    current_level = 2;
}

void level_load3(Player &player, sf::Texture &ground_texture, sf::Texture &enemy_texture, sf::Texture &star_texture)
{

    int starting_pos[2] = {0, 0};
    player.x = starting_pos[0];
    player.y = starting_pos[1];
    player.start_pos_x = starting_pos[0];
    player.start_pos_y = starting_pos[1];

    background.loadFromFile("assets/background4b.png");
    background_sprite.setTexture(background);

    for (int i = 0; i < 3; i++)
    {
        add_ground(ground_texture, i * 24, 168);
    }

    for (int i = -20; i < 15; i++)
    {
        add_ground(ground_texture, 192, i * 24);
    }

    for (int i = 0; i < 9; i++)
    {
        add_ground(ground_texture, i * 24, 768);
    }
    for (int i = 0; i < 9; i++)
    {
        add_ground(ground_texture, i * 24, 744);
    }
    for (int i = 0; i < 9; i++)
    {
        add_ground(ground_texture, i * 24, 792);
    }

    for (int i = 13; i < 17; i++)
    {
        add_ground(ground_texture, i * 24, 528);
    }
    for (int i = 0; i < 3; i++)
    {
        add_ground(ground_texture, i * 24, 168);
    }
    for (int i = 30; i < 45; i++)
    {
        add_ground(ground_texture, i * 24, 720);
    }
    for (int i = 38; i < 43; i++)
    {
        add_ground(ground_texture, i * 24, 528);
    }

    for (int i = 29; i < 33; i++)
    {
        add_ground(ground_texture, i * 24, 433);
    }

    for (int i = 38; i < 43; i++)
    {
        add_ground(ground_texture, i * 24, 338);
    }
    for (int i = 25; i < 33; i++)
    {
        add_ground(ground_texture, i * 24, 192);
    }
    for (int i = 9; i < 20; i++)
    {
        add_ground(ground_texture, i * 24, 144);
    }
    add_enemy(enemy_texture, 24, 650, 0, 0);
    add_enemy(enemy_texture, 600, 120, 0, 0);
    add_enemy(enemy_texture, 790, 650, 3, 30);
    add_enemy(enemy_texture, 270, 100, 4, 0);

    add_star(star_texture, 248, 72);
    add_star(star_texture, 312, 72);
    add_star(star_texture, 312, 240);
    add_star(star_texture, 248, 72);
    add_star(star_texture, 600, 120);
    add_star(star_texture, 930, 264);
    add_star(star_texture, 670, 360);
    add_star(star_texture, 312, 240);
    add_star(star_texture, 72, 384);
    add_star(star_texture, 96, -40);
    add_star(star_texture, 248, 72);
    add_star(star_texture, 864, 648);

    current_level = 3;
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(1024, 800),
        "Speed Star");
    window.setFramerateLimit(60);

    float time_to_live_player = 65;
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
                player.x = player.start_pos_x;
                player.y = player.start_pos_y;
                player.vidas--;
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
                        time_life.restart();
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
                        level_load3(player, ground_texture, enemy_texture, star_texture);
                        time_life.restart();
                        continue;
                    }
                    else if (current_level == 3)
                    {
                        // show "Vitoria"
                        window.clear();
                        sf::Text victoryText;
                        victoryText.setFont(font);
                        victoryText.setCharacterSize(48);
                        victoryText.setFillColor(sf::Color::Green);
                        victoryText.setString("Vitoria!");
                        victoryText.setPosition(window.getSize().x / 2 - victoryText.getLocalBounds().width / 2, window.getSize().y / 2 - victoryText.getLocalBounds().height / 2);
                        window.draw(victoryText);
                        window.display();
                        sf::sleep(sf::seconds(10)); // Display victory text for 3 seconds
                        return 0;
                    }
                }
            }
        }

        if (player.vidas == 0)
        {
            window.clear();
            // show "Derrota"
            sf::Text defeatText;
            defeatText.setFont(font);
            defeatText.setCharacterSize(48);
            defeatText.setFillColor(sf::Color::Red);
            defeatText.setString("Derrota!");
            defeatText.setPosition(window.getSize().x / 2 - defeatText.getLocalBounds().width / 2, window.getSize().y / 2 - defeatText.getLocalBounds().height / 2);
            window.draw(defeatText);
            window.display();
            sf::sleep(sf::seconds(10)); // Display defeat text for 3 seconds
            return 0;
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
        text.setString("Time: " + timeString + "   Vidas: " + std::to_string(player.vidas) + "   Level: " + std::to_string(current_level) + "   Stars: " + std::to_string(stars_found) + "/" + std::to_string(star_count));
        if (time_to_live_player - time_life.getElapsedTime().asSeconds() < 0)
        {
            return 0;
        }

        // ver posi�ao blocos

        sf::Vector2i cursorPosition = sf::Mouse::getPosition(window);
        printf("X: %d Y: %d\n", cursorPosition.x / 24 * 24, cursorPosition.y / 24 * 24);

        // DRAW
        window.clear();
        // set the background to 1024 800
        background_sprite.setScale(1024 / background_sprite.getLocalBounds().width, 800 / background_sprite.getLocalBounds().height);
        window.draw(background_sprite);
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