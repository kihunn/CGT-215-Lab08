#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;



void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

PhysicsSprite& CreateBalloon(PhysicsShapeList<PhysicsSprite>& balloons,Texture& redTex, World& world) {
    PhysicsSprite& balloon = balloons.Create();
    balloon.setTexture(redTex);
    int x = 50;
    Vector2f sz = balloon.getSize();
    balloon.setCenter(Vector2f(x, 20 + (sz.y / 2)));
    balloon.setVelocity(Vector2f(0.25, 0));
    world.AddPhysicsBody(balloon);
    return balloon;
}


int main()
{
    RenderWindow window(VideoMode(800, 600), "Balloon Buster");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);
    int boltsFired = 0;

    PhysicsSprite crossBow;
    Texture cbowTex;
    LoadTex(cbowTex, "crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400, 600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);
    Texture redTex;
    LoadTex(redTex, "duck.png");
    PhysicsShapeList<PhysicsSprite> balloons;
    

    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
        };

    Text scoreText;
    Font font;
    if (!font.loadFromFile("arial1.ttf")) {
        cout << "Couldn't load font arial.ttf" << endl;
        exit(1);
    }
    scoreText.setFont(font);
    Text arrowCountText;
    arrowCountText.setFont(font);

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);
    //Time balloonCreationTime = seconds(2); // Time interval between balloon creation

    // Inside the main loop
    long balloonSpawnTime(0);


    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
     
        balloonSpawnTime += deltaMS; // Increment our balloon counter

        if (deltaMS > 9) {
            cout << deltaMS << endl;
            if (balloonSpawnTime >= 20000000) {
                //cout << balloonSpawnTime << endl;
                balloonSpawnTime = 0;
                PhysicsSprite& balloon = CreateBalloon(balloons, redTex, world);
                balloon.onCollision =
                    [&drawingArrow, &world, &arrow, &balloon, &balloons, &score]
                    (PhysicsBodyCollisionResult result) {
                        if (result.object2 == arrow) {
                            drawingArrow = false;
                            world.RemovePhysicsBody(arrow);
                        }
                    };

            }
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);

            

            if (Keyboard::isKeyPressed(Keyboard::Space) && arrows > 0 && !drawingArrow) {
                drawingArrow = true;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);
                boltsFired++;
                arrows--;

                if (boltsFired >= 5) {
                    break;
                }
            }

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            for (PhysicsShape& balloon : balloons) {
                window.draw((PhysicsSprite&)balloon);
            }
            window.draw(crossBow);
            scoreText.setString(to_string(score));
            FloatRect textBounds = scoreText.getGlobalBounds();
            scoreText.setPosition(
                Vector2f(790 - textBounds.width, 590 - textBounds.height));
            window.draw(scoreText);
            arrowCountText.setString(to_string(arrows));
            textBounds = arrowCountText.getGlobalBounds();
            arrowCountText.setPosition(
                Vector2f(10, 590 - textBounds.height));
            window.draw(arrowCountText);
            window.display();
            balloons.DoRemovals();
        }
    }

    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    FloatRect textBounds = gameOverText.getGlobalBounds();
    gameOverText.setPosition(Vector2f(
        400 - (textBounds.width / 2),
        300 - (textBounds.height / 2)
    ));
    window.draw(gameOverText);
    window.display();
    while (true);

    return 0;
}
