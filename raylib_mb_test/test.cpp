#define _CRT_SECURE_NO_WARNINGS 1
#include<raylib.h>
#include<deque>
#include<raymath.h>
using namespace std;
Color darkGreen = { 255, 210, 60, 255 };
int cellSize = 30;
int cellCount = 25;
int offset = 75;

const double INITIAL_INTERVAL = 0.2;
const double MIN_INTERVAL = 0.05;
const double INTERVAL_DECREASE = 0.015;
const int SPEED_UP_SCORE = 10;

bool ElementInDeque(Vector2 element, const deque<Vector2>& dq)
{
	for (unsigned int i = 0; i < dq.size(); i++)
	{
		if (Vector2Equals(element, dq[i]))
		{
			return true;
		}
	}
	return false;
}

class Snake
{
	bool addSegment = false;
	Vector2 direction = { 1,0 };
	Vector2 nextDirection = { 1,0 };
	Texture2D headTexture;
	deque<Vector2> body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };

public:
	Snake()
	{
		Image image = LoadImage("head.png");
		if (image.data == NULL)
		{
			TraceLog(LOG_FATAL, "Failed to load head image - game cannot start");
			exit(1);
		}
		headTexture = LoadTextureFromImage(image);
		UnloadImage(image);
	}

	~Snake()
	{
		UnloadTexture(headTexture);
	}

	Vector2 GetDirection() const
	{
		return direction;
	}

	const deque<Vector2>& GetBody() const
	{
		return body;
	}

	void SetDirection(Vector2 dir)
	{
		if (dir.x != -direction.x || dir.y != -direction.y)
		{
			nextDirection = dir;
		}
	}

	void Draw()
	{
		for (unsigned int i = 0; i < body.size(); i++)
		{
			float x = body[i].x;
			float y = body[i].y;
			Vector2 pos = { offset + x * cellSize, offset + y * cellSize };
			float scale = (float)cellSize / (float)fmax(headTexture.width, headTexture.height);
			if (i == 0)
				DrawTextureEx(headTexture, pos, 0.0f, scale, WHITE);
			else
				DrawRectangleRounded(Rectangle{ pos.x, pos.y, (float)cellSize, (float)cellSize }, 0.5, 6, darkGreen);
		}
	}

	void Update()
	{
		direction = nextDirection;
		body.push_front(Vector2Add(body[0], direction));

		if (addSegment)
		{
			addSegment = false;
		}
		else
		{
			body.pop_back();
		}
	}

	void AddSegmentFlag()
	{
		addSegment = true;
	}

	void Reset()
	{
		body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
		direction = { 1,0 };
		nextDirection = { 1,0 };
		addSegment = false;
	}
};

class Food
{
public:
	Vector2 position;
	Texture2D texture;

	Food(const deque<Vector2>& snakeBody)
	{
		Image image = LoadImage("food.png");
		if (image.data == NULL)
		{
			TraceLog(LOG_FATAL, "Failed to load image - game cannot start");
			exit(1);
		}
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = GenerateRandomPos(snakeBody);
	}

	~Food()
	{
		UnloadTexture(texture);
	}

	void Draw()
	{
		float scale = (float)cellSize / (float)fmax(texture.width, texture.height);
		DrawTextureEx(texture, { offset + position.x * cellSize, offset + position.y * cellSize }, 0.0f, scale, WHITE);
	}

	Vector2 GenerateRandomCell()
	{
		float x = (float)GetRandomValue(0, cellCount - 1);
		float y = (float)GetRandomValue(0, cellCount - 1);
		return Vector2{ x, y };
	}

	Vector2 GenerateRandomPos(const deque<Vector2>& snakeBody)
	{
		if ((int)snakeBody.size() >= cellCount * cellCount)
		{
			return { -1, -1 };
		}
		Vector2 pos = GenerateRandomCell();
		while (ElementInDeque(pos, snakeBody))
		{
			pos = GenerateRandomCell();
		}
		return pos;
	}
};

class Game
{
	Texture2D bgTexture;
	double lastUpdateTime = 0;

public:
	bool running = true;
	Snake snake = Snake();
	int score = 0;
	double interval = INITIAL_INTERVAL;
	Food food = Food(snake.GetBody());

	Game()
	{
		Image image = LoadImage("background.png");
		if (image.data == NULL)
		{
			TraceLog(LOG_FATAL, "Failed to load background image - game cannot start");
			exit(1);
		}
		bgTexture = LoadTextureFromImage(image);
		UnloadImage(image);
	}

	~Game()
	{
		UnloadTexture(bgTexture);
	}

	bool EventTriggered()
	{
		double currentTime = GetTime();
		if (currentTime - lastUpdateTime >= interval)
		{
			lastUpdateTime = currentTime;
			return true;
		}
		return false;
	}

	void Restart()
	{
		snake.Reset();
		food.position = food.GenerateRandomPos(snake.GetBody());
		score = 0;
		interval = INITIAL_INTERVAL;
		running = true;
	}

	void Draw()
	{
		float destSize = (float)cellSize * cellCount;
		float imgW = (float)bgTexture.width;
		float imgH = (float)bgTexture.height;
		float destOffset = (float)offset;
		float scale = fmax(destSize / imgW, destSize / imgH);
		Rectangle src = {
			(imgW - destSize / scale) * 0.5f,
			(imgH - destSize / scale) * 0.5f,
			destSize / scale,
			destSize / scale
		};
		DrawTexturePro(bgTexture, src,
			Rectangle{ destOffset, destOffset, destSize, destSize },
			{ 0, 0 }, 0.0f, WHITE);
		DrawRectangleLinesEx(Rectangle{ (float)offset - 5,(float)offset - 5,(float)cellSize * cellCount + 10,(float)cellSize * cellCount + 10 }, 5, darkGreen);
		DrawText("Snake Game", offset - 5, 20, 40, darkGreen);
		DrawText(TextFormat("Score: %i", score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
		snake.Draw();
		food.Draw();
		if (!running)
		{
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0,0,0,180 });
			DrawText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 60) / 2, GetScreenHeight() / 2 - 60, 60, WHITE);
			DrawText(TextFormat("Score: %i", score), GetScreenWidth() / 2 - MeasureText(TextFormat("Score: %i", score), 30) / 2, GetScreenHeight() / 2 + 10, 30, WHITE);
			DrawText("Press any direction to restart", GetScreenWidth() / 2 - MeasureText("Press any direction to restart", 20) / 2, GetScreenHeight() / 2 + 50, 20, WHITE);
		}
	}

	void Update()
	{
		if (running)
		{
			snake.Update();
			CheckCollisionWithFood();
			CheckCollisionWithEdge();
			CheckCollisionWithTail();
		}
	}

	void CheckCollisionWithFood()
	{
		if (Vector2Equals(snake.GetBody()[0], food.position))
		{
			food.position = food.GenerateRandomPos(snake.GetBody());
			snake.AddSegmentFlag();
			++score;
			if (score % SPEED_UP_SCORE == 0 && interval > MIN_INTERVAL)
			{
				interval -= INTERVAL_DECREASE;
			}
		}
	}

	void CheckCollisionWithEdge()
	{
		if (snake.GetBody()[0].x == cellCount || snake.GetBody()[0].x == -1)
		{
			GameOver();
		}
		if (snake.GetBody()[0].y == cellCount || snake.GetBody()[0].y == -1)
		{
			GameOver();
		}
	}

	void GameOver()
	{
		snake.Reset();
		food.position = food.GenerateRandomPos(snake.GetBody());
		running = false;
	}

	void CheckCollisionWithTail()
	{
		deque<Vector2> headlessBody = snake.GetBody();
		headlessBody.pop_front();
		if (ElementInDeque(snake.GetBody()[0], headlessBody))
		{
			GameOver();
		}
	}
};

int main()
{
	InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Snake Game");
	InitAudioDevice();
	Music music = LoadMusicStream("bgm.ogg");
	if (music.frameCount == 0)
	{
		TraceLog(LOG_FATAL, "Failed to load background music");
		CloseAudioDevice();
		CloseWindow();
		return 1;
	}
	PlayMusicStream(music);
	SetTargetFPS(60);
	Game game = Game();
	while (WindowShouldClose() == false)
	{
		UpdateMusicStream(music);
		BeginDrawing();
		ClearBackground(RAYWHITE);

		if (game.EventTriggered())
		{
			game.Update();
		}

		if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
		{
			if (!game.running) game.Restart();
			game.snake.SetDirection({ 0,-1 });
		}

		if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
		{
			if (!game.running) game.Restart();
			game.snake.SetDirection({ 0,1 });
		}

		if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
		{
			if (!game.running) game.Restart();
			game.snake.SetDirection({ -1,0 });
		}

		if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
		{
			if (!game.running) game.Restart();
			game.snake.SetDirection({ 1,0 });
		}

		game.Draw();
		EndDrawing();
	}
	CloseWindow();
	UnloadMusicStream(music);
	CloseAudioDevice();
	return 0;
}
