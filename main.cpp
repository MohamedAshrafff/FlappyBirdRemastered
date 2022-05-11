#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>
using namespace std;
using namespace sf;




struct Pipe
{
    float pos;
    float height;
};

//vairables
float WindowHeight = 760;
float WindowWidth = 500;
float speed = 3;
float Pipe_space = 300;
float gap = 200;
float PipeStart = 600;
int counter = 0;
long long score = 0;
long long best = 0;
String state = "MainMenu";
bool hasCollided = true;
float SafeD = 150;
int current_pipe = 0;
bool shouldFall = false;

long long minimum_score = 100000;

//main menu choosing easy or hard
void InitVariables(bool easy) {
    if (easy) {
        speed = 6;
        Pipe_space = 300;
        gap = 200;
    }
    else {
        speed = 10;
        Pipe_space = 300;
        gap = 170;
    }
}


//declaration of textures windows and sounds 
sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), "Flappy Bird");
SoundBuffer WingBuff, PointBuff, HitBuff, DieBuff;
Sound  WingSound, PointSound, HitSound, DieSound;
Texture  BaseTexture, BackGroundTexture, LogoTexture, PipeTexture, ScoreBoardTexture, Button1Texture, Button2Texture;
Sprite Base2Sprite, Base1Sprite, BackGround2Sprite, BackGround1Sprite, LogoSprite, PipeSprite, ScoreBoardSprite, Button1Sprite, Button2Sprite;
vector < Pipe > Pipes;

//generates pipe heights 
float genarate_height(float gap, float win_height, float inc)
{
    int range = win_height - inc;
    int min = inc + gap;
    return min + (rand() % (range - min + 1));

}

// main menu choices and functions calling and pausing 
void GoToMainMenu()
{
    window.clear();
    window.draw(BackGround1Sprite);
    window.draw(LogoSprite);
    window.draw(ScoreBoardSprite);

    //current score in main menu
    sf::Font font;
    if (!font.loadFromFile("font.TTF")) {}
    sf::Text text1(to_string(score), font);
    text1.setFont(font);
    text1.setCharacterSize(48);
    text1.setFillColor(sf::Color::White);
    text1.setPosition(235, 220);
    window.draw(text1);

    //best score in main menu
    sf::Text best1(to_string(best), font);
    best1.setFont(font);
    best1.setCharacterSize(48);
    best1.setFillColor(sf::Color::White);
    best1.setPosition(235, 300);
    window.draw(best1);

    //continue sentence display
    if (state == "Pause")
    {
        sf::Text continueit;
        continueit.setFont(font);
        continueit.setString(" press SPACEBAR \n       to continue ");
        continueit.setCharacterSize(35);
        continueit.setPosition(75, 390);
        continueit.setFillColor(sf::Color::White);
        window.draw(continueit);
    }
    //buttons to display on main menu and not while pausing
    if (state != "Pause")
    {
        window.draw(Button1Sprite);
        window.draw(Button2Sprite);
    }
    window.display();

}

//game logic
struct Bird
{
// ismael 
private: 
    float rotationAngle = 0;
    const float scaleFactor = 0.037795f;     //for converting real life meters to suitable pixels
    const float fallingSpeed = 9.81f * (1 / (float)60) * (scaleFactor * 100);   //Speed gained by gravity = g x t , where t = 1 / fps
    const float maxSpeed = 12;
    const float speedGainedWhenJump = -sqrtf(2 * 9.81f * 125 * scaleFactor);  //Speed gained by jumping = squareRoot( 2 x g x displacement )
    float groundHeight;
    float avDiameter;
    bool isFlying = false;
    bool isPressing = false;
    int currentFrameIndex = 1;
    int frames = 0;
    int framesLeftToFly = 0;
    sf::IntRect uvRect;
    sf::Texture texture;
    sf::Vector2f movementVelocity;
    sf::RectangleShape rect;

public:

    //setting origin of bird and setting its initial position to draw
    Bird(float sizeX, float sizeY)
    {
        rect = sf::RectangleShape(sf::Vector2f(sizeX, sizeY));
        rect.setOrigin(sf::Vector2f(sizeX / 2, sizeY / 2));
        texture.loadFromFile("img/bird.png");
        rect.setTexture(&texture);
        uvRect.width = texture.getSize().x / 3;
        uvRect.height = texture.getSize().y;
        uvRect.left = currentFrameIndex * uvRect.width;
        rect.setTextureRect(uvRect);

        movementVelocity = sf::Vector2f(0, 0);

        avDiameter = (sizeX + sizeY) / 4;

    }
    // ismael
    //resets bird position while starting the game
    void resetPosition()
    {
        rect.setPosition(sf::Vector2f(window.getSize().x / 3, window.getSize().y / 2));
    }

    void draw()
    {
        // ismael
        frames++;
        //flying the bird with clicking a single space 
        if (!hasCollided && (Keyboard::isKeyPressed(sf::Keyboard::Space) || Mouse::isButtonPressed(sf::Mouse::Button::Left)))
        {
            if (!isPressing)
            {
                WingSound.play();
                isFlying = true;
                isPressing = true;
                framesLeftToFly = 5;
                movementVelocity.y = speedGainedWhenJump;
            }

        }
        else
        {
            isPressing = false;
        }
        
        //falling movement
        movementVelocity.y += fallingSpeed;
        rotationAngle += 5;

        //wings animation per second
        if (frames >= 8)
        {
            frames = 0;
            //index in sprite to animate
            if (isFlying)
            {
                framesLeftToFly--;
                currentFrameIndex++;


                if (framesLeftToFly <= 0)
                {
                    isFlying = false;
                    currentFrameIndex = 1;
                }
                else if (currentFrameIndex >= 3)
                {
                    currentFrameIndex = 0;
                }
            }
            else
            {
                currentFrameIndex = 1;
            }


            uvRect.left = currentFrameIndex * uvRect.width;
            rect.setTextureRect(uvRect);
        }

        if (isFlying)
        {
            rotationAngle = -20;
        }

        if (rotationAngle > 90)
        {
            rotationAngle = 90;
        }
        if (movementVelocity.y > maxSpeed)
        {
            movementVelocity.y = maxSpeed;
        }

        if (!hasCollided)
        {
            rect.setRotation(rotationAngle);
            rect.move(movementVelocity);
        }


        //jandar 
        // check collision

        if (!hasCollided && rect.getPosition().y + avDiameter > Base1Sprite.getPosition().y)
        {
            hasCollided = true;
            HitSound.play();
            DieSound.play();
            state = "MainMenu";

        }


        if (!hasCollided && Pipes.size() > 0)
        {
            if (rect.getPosition().x - 15 >= Pipes.at(current_pipe).pos - rect.getGlobalBounds().width / 2 && rect.getPosition().x <= Pipes.at(current_pipe).pos + PipeSprite.getGlobalBounds().width)
            {
                if (rect.getPosition().y >= Pipes.at(current_pipe).height - rect.getGlobalBounds().height / 2 + 4 || rect.getPosition().y <= Pipes.at(current_pipe).height - gap + 24)
                {
                    // cout << "COLLIDE!\n";
                    hasCollided = true;
                    HitSound.play();
                    shouldFall = true;

                }
            }
        }

        //Check Score
        if (Pipes.back().pos <= WindowWidth - Pipe_space) {
            Pipes.push_back({ Pipes.at(current_pipe).pos + Pipe_space, genarate_height(gap,WindowHeight,SafeD) });
        }

        //score counter checker 
        if (Pipes.size() > 0)
        {
            if (rect.getPosition().x > Pipes.at(current_pipe).pos + PipeSprite.getGlobalBounds().width)
            {
                score++;
                current_pipe++;
                PointSound.play();
            }
        }
    
        //collision falling animation
        if (shouldFall)
        {
            rect.setRotation(90.f);
            rect.move(movementVelocity);
            if (rect.getPosition().y + avDiameter > 700)
            {
                DieSound.play();
                shouldFall = false;
                state = "MainMenu";
            }
        }
        //Mohamed Ashraf
        //storing best score value
        if (score > best)
            best = score;

        if (score < minimum_score && score  >0 && hasCollided)
        {
            minimum_score = score ;
            cout << minimum_score<<endl; 
        }

        // displaying score counter while playing
        sf::Font font;
        if (!font.loadFromFile("font.TTF")) {}
        sf::Text text(to_string(score), font);
        text.setFont(font);
        text.setCharacterSize(72);
        text.setFillColor(sf::Color::White);
        text.setPosition(225, 100);
        window.draw(text);
        //displaying how to pause sentence 
        sf::Text pauseit;
        pauseit.setFont(font);
        pauseit.setString(" P to pause ");
        pauseit.setCharacterSize(35);
        pauseit.setFillColor(sf::Color::Black);
        window.draw(pauseit);
        window.draw(text);
        window.draw(rect);
    }

};

//Declare bird size (as pixels) as global

Bird bird(69, 48);
//abdelmageed
//function to start game normally or if collided 
void startGame()
{
    shouldFall = false;
    state = "Game";
    counter = 0;
    score = 0;
    current_pipe = 0;
    Pipes.clear();
    Pipes.push_back(Pipe{ PipeStart,genarate_height(gap,WindowHeight,SafeD) });
    hasCollided = false;
    bird.resetPosition();

}

//loading textures and sounds
void LoadFiles()
{
    DieBuff.loadFromFile("Sounds/sfx_die.wav");
    HitBuff.loadFromFile("Sounds/sfx_hit.wav");
    PointBuff.loadFromFile("Sounds/sfx_point.wav");
    WingBuff.loadFromFile("Sounds/sfx_wing.wav");
    PipeTexture.loadFromFile("img/pipe.png");
    LogoTexture.loadFromFile("img/logo.png");
    BackGroundTexture.loadFromFile("img/background.png");
    BaseTexture.loadFromFile("img/base2.jpg");
    ScoreBoardTexture.loadFromFile("img/score_board.png");
    Button1Texture.loadFromFile("img/easy.png");
    Button2Texture.loadFromFile("img/hard.png");
}

//setting textures 
void SetTextures()
{
    DieSound.setBuffer(DieBuff);
    HitSound.setBuffer(HitBuff);
    PointSound.setBuffer(PointBuff);
    WingSound.setBuffer(WingBuff);
    PipeSprite.setTexture(PipeTexture);
    LogoSprite.setTexture(LogoTexture);
    BackGround1Sprite.setTexture(BackGroundTexture);
    BackGround2Sprite.setTexture(BackGroundTexture);
    Base1Sprite.setTexture(BaseTexture);
    Base2Sprite.setTexture(BaseTexture);
    ScoreBoardSprite.setTexture(ScoreBoardTexture);
    Button1Sprite.setTexture(Button1Texture);
    Button2Sprite.setTexture(Button2Texture);

}

//setting textures positions through game  
void SettingPositions()
{
    PipeSprite.setScale(2, 3);
    LogoSprite.setOrigin(50, 13);
    LogoSprite.setPosition(WindowWidth / 2, WindowHeight / 8);
    LogoSprite.setScale(2.3, 2);
    ScoreBoardSprite.setOrigin(108, 143);
    ScoreBoardSprite.setPosition(WindowWidth / 2, WindowHeight / 2.8);
    ScoreBoardSprite.setScale(0.65, 0.65);
    Button1Sprite.setOrigin(134, 46);
    Button2Sprite.setOrigin(134, 46);
    Button1Sprite.setPosition(WindowWidth / 2 - (WindowWidth / 5), WindowHeight / 1.7);
    Button2Sprite.setPosition(WindowWidth / 2 + (WindowWidth / 5), WindowHeight / 1.7);
    Button1Sprite.setScale(0.5, 0.5);
    Button2Sprite.setScale(0.5, 0.5);
    BackGround2Sprite.setPosition(1024, 0);
    Base1Sprite.setPosition(0, WindowHeight - Base1Sprite.getGlobalBounds().height);
    Base2Sprite.setPosition(1023, WindowHeight - Base1Sprite.getGlobalBounds().height);
}

//jandar
//check if buttons on screen clicked or not
bool isSpriteHover(sf::FloatRect sprite, sf::Vector2f mp)
{
    if (sprite.contains(mp))
    {
        return true;
    }
    else
        return false;
}


// buttons and window functions 
void WindowEvents() {
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (Keyboard::isKeyPressed(Keyboard::P) && !hasCollided)
        {
            state = "Pause";
        }
        if (Keyboard::isKeyPressed(Keyboard::Space) && state == "Pause")
        {
            state = "Game";
        }
        if (Keyboard::isKeyPressed(Keyboard::Q))
        {
            exit(0);
        }
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        {
            if (isSpriteHover(Button1Sprite.getGlobalBounds(), sf::Vector2f(event.mouseButton.x, event.mouseButton.y)) && state == "MainMenu")
            {

                InitVariables(true);
                startGame();
            }
            if (isSpriteHover(Button2Sprite.getGlobalBounds(), sf::Vector2f(event.mouseButton.x, event.mouseButton.y)) && state == "MainMenu")
            {

                InitVariables(false);
                startGame();
            }
        }


    }
}

//jander-Ashraf
//moving ground and base 
void MoveBase()
{
    Base1Sprite.move(-speed, 0);
    Base2Sprite.move(-speed, 0);
    if (Base1Sprite.getPosition().x <= -1023)
    {
        Base1Sprite.setPosition(0, 700);
        Base2Sprite.setPosition(1023, 700);
    }
}

//jandar 
//moving pipes and generating it
void MovePipes()
{
    for (int i = 0; i < Pipes.size(); i++)
    {
        Pipe& current = Pipes.at(i);
        PipeSprite.setPosition(current.pos, current.height);
        window.draw(PipeSprite);
        PipeSprite.setRotation(180);
        PipeSprite.setPosition(current.pos + 52 * 2, current.height - gap);
        window.draw(PipeSprite);
        PipeSprite.setRotation(0);

        if (!hasCollided)
        {
            current.pos -= speed;
        }

    }
}


//calling our functions and starting game 
int main()
{


    LoadFiles();
    SetTextures();
    SettingPositions();

    Pipes.push_back(Pipe{ PipeStart,genarate_height(gap,WindowHeight,SafeD) });

    window.setFramerateLimit(60);
    while (window.isOpen())
    {
        WindowEvents();

        if (state == "Game")
        {

            if (!hasCollided && state != "Pause")
            {
                MoveBase();
            }

            window.clear();
            window.draw(BackGround1Sprite);

            MovePipes();

            window.draw(Base1Sprite);
            window.draw(Base2Sprite);
            bird.draw();
            window.display();
        }
        if (state == "MainMenu" || state == "Pause")
        {
            //cout << "meu\n"; /
            GoToMainMenu();
            //Menu Should be Displayed Here
        }

    }
    return 0;
}









