#include<windows.h>
#ifdef APPLE
#include<Glut/glut.h>
#else
#include<GL/glut.h>
#endif
#include<stdio.h>
#include<math.h>
#include<string.h>

// Ball properties
float ballX = 0.0, ballY = -8.0;
float ballSpeedX = 0.0, ballSpeedY = 0.5;
bool ballMoving = false;

// Goalkeeper properties
float goalieX = 0.0;
float goalieSpeed = 0.20;
float goalWidth = 8.0;

// Game state
bool gameEnded = false;
bool scored = false;
bool matchEnded = false; // To track if the match is over
char winner[50]; // To store the winner message
int playerTurn = 1; // Track player turn (1 or 2)
int player1Score = 0, player2Score = 0;

// Function to render text
void renderText(float x, float y, const char *string, void *font) {
    glRasterPos2f(x, y);
    for (int i = 0; i < strlen(string); i++) {
        glutBitmapCharacter(font, string[i]);
    }
}

// Draw a human figure (for player and goalkeeper)
void drawHuman(float centerX, float centerY, float scale, bool isGoalkeeper) {
    glPushMatrix();
    glTranslatef(centerX, centerY, 0);
    glScalef(scale, scale, 1.0);

    // Draw head
    glColor3f(1.0, 0.8, 0.6);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++) {
        float angle = 2.0f * M_PI * i / 100;
        glVertex2f(0.3 * cos(angle), 0.3 * sin(angle) + 1.2);
    }
    glEnd();

    // Draw body
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(-0.3, 0.8);
    glVertex2f(0.3, 0.8);
    glVertex2f(0.2, -0.5);
    glVertex2f(-0.2, -0.5);
    glEnd();

    // Arms
    glColor3f(1.0, 0.8, 0.6);
    glBegin(GL_QUADS);
    glVertex2f(-0.6, 0.6);
    glVertex2f(-0.3, 0.6);
    glVertex2f(-0.3, 0.3);
    glVertex2f(-0.6, 0.3);

    glVertex2f(0.3, 0.6);
    glVertex2f(0.6, 0.6);
    glVertex2f(0.6, 0.3);
    glVertex2f(0.3, 0.3);
    glEnd();

    // Legs
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(-0.2, -0.5);
    glVertex2f(-0.1, -0.5);
    glVertex2f(-0.1, -1.0);
    glVertex2f(-0.2, -1.0);

    glVertex2f(0.1, -0.5);
    glVertex2f(0.2, -0.5);
    glVertex2f(0.2, -1.0);
    glVertex2f(0.1, -1.0);
    glEnd();

    glPopMatrix();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw background
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.5, 0.0);
    glVertex2f(-10, -10);
    glVertex2f(10, -10);
    glColor3f(0.5, 1.0, 0.5);
    glVertex2f(10, 10);
    glVertex2f(-10, 10);
    glEnd();

    // Draw the D-box
    float penaltyBoxWidth = 12.0;
    float penaltyBoxHeight = 16.0;
    glBegin(GL_LINE_LOOP);
    glVertex2f(-penaltyBoxWidth / 2, 8.0);
    glVertex2f(penaltyBoxWidth / 2, 8.0);
    glVertex2f(penaltyBoxWidth / 2, 8.0 - penaltyBoxHeight);
    glVertex2f(-penaltyBoxWidth / 2, 8.0 - penaltyBoxHeight);
    glEnd();

    // Draw goalpost
    float postThickness = 0.2;
    float goalHeight = 1.5;
    float postOffset = 0.5;

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(-goalWidth / 2 - postThickness / 2, 8.0 + postOffset);
    glVertex2f(-goalWidth / 2 + postThickness / 2, 8.0 + postOffset);
    glVertex2f(-goalWidth / 2 + postThickness / 2, 8.0 + postOffset - goalHeight);
    glVertex2f(-goalWidth / 2 - postThickness / 2, 8.0 + postOffset - goalHeight);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(goalWidth / 2 - postThickness / 2, 8.0 + postOffset);
    glVertex2f(goalWidth / 2 + postThickness / 2, 8.0 + postOffset);
    glVertex2f(goalWidth / 2 + postThickness / 2, 8.0 + postOffset - goalHeight);
    glVertex2f(goalWidth / 2 - postThickness / 2, 8.0 + postOffset - goalHeight);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(-goalWidth / 2 - postThickness / 2, 8.0 + postOffset);
    glVertex2f(goalWidth / 2 + postThickness / 2, 8.0 + postOffset);
    glVertex2f(goalWidth / 2 + postThickness / 2, 8.0 + postOffset + postThickness);
    glVertex2f(-goalWidth / 2 - postThickness / 2, 8.0 + postOffset + postThickness);
    glEnd();

    // Draw goalkeeper
    drawHuman(goalieX, 6.5, 0.8, true);

    // Draw player
    drawHuman(0, -8.0, 0.8, false);


// Draw ball
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < 100; i++) {
        float angle = 2.0f * M_PI * i / 100;
        float x = ballX + 0.3 * cos(angle);
        float y = ballY + 0.3 * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    // Display scores
    char scoreText[50];
    sprintf(scoreText, "Player 1: %d | Player 2: %d", player1Score, player2Score);
    glColor3f(1.0, 1.0, 1.0);
    renderText(-9.0, 9.0, scoreText, GLUT_BITMAP_HELVETICA_18);

    // Game result messages
    if (gameEnded) {
        const char *result = scored ? "GOAL!!!" : "BLOCKED!!!";
        renderText(-2.0, 0.0, result, GLUT_BITMAP_HELVETICA_18);
    }

    if (matchEnded) {
    glColor3f(1.0, 1.0, 0.0);
    renderText(4.0, 9.0, winner, GLUT_BITMAP_HELVETICA_18); // Position changed to right top
}

    glFlush();
}

void update(int value) {
    if (matchEnded) {
        glutPostRedisplay();
        return;
    }

    // Move goalkeeper
    goalieX += goalieSpeed;
    if (goalieX > goalWidth / 2 - 1 || goalieX < -goalWidth / 2 + 1) {
        goalieSpeed = -goalieSpeed;
    }

    // Ball movement
    if (ballMoving && !gameEnded) {
        ballY += ballSpeedY;

        // Collision with goalkeeper
        if (ballY >= 6.5 && ballY <= 7.0 && ballX >= goalieX - 1.0 && ballX <= goalieX + 1.0) {
            gameEnded = true;
            scored = false;
        }

        // Goal scored
        if (ballY >= 7.0) {
            gameEnded = true;
            scored = true;
            if (playerTurn == 1) player1Score++;
            else player2Score++;
        }
    }

    // Determine if match ends
        // Determine if match ends
    if (abs(player1Score - player2Score) >= 2) {
        matchEnded = true;
        if (player1Score > player2Score) {
            sprintf(winner, "Player 1 Wins!");
        } else {
            sprintf(winner, "Player 2 Wins!");
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == ' ') { // Shoot
        if (!ballMoving && !gameEnded && !matchEnded) ballMoving = true;
    }
    if (key == 'r') { // Reset
        if (!matchEnded) {
            ballX = 0.0;
            ballY = -8.0;
            ballMoving = false;
            gameEnded = false;
            scored = false;
            playerTurn = (playerTurn == 1) ? 2 : 1;
        }
    }
    if (key == 'n') { // New match
        if (matchEnded) {
            matchEnded = false;
            player1Score = 0;
            player2Score = 0;
            ballX = 0.0;
            ballY = -8.0;
            ballMoving = false;
            gameEnded = false;
            scored = false;
            playerTurn = 1;
        }
    }
}

void init(void) {
    glClearColor(0.0, 0.5, 0.8, 0.0);
    glOrtho(-10, 10, -10, 10, -10, 10);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Multiplayer Penalty Shootout");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
