#include <SDL.h>
#include <stdio.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <SDL_image.h>
#include "SDL2_rotozoom.h"
#include <mutex>
#include <thread>
#include <list>
#include <map>
#include "dirent.h" //For directory traversal
#include <regex>
#include <fstream>
#include <iostream>
#include "SDL2_gfxPrimitives.h"

#include "Entity.h"
#include "Creature.h"
#include "Carnivore.h"
#include "Omnivore.h"
#include "Herbivore.h"
#include "Plant.h"

#include "WorldConstants.h"

#include <time.h>
#include <condition_variable>

#define RANDOMFLOAT ((float)rand() / RAND_MAX)

#include "Image.h"
#include "DisplayableImage.h"


#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define DRAW_QUADTREE_DEBUG true

#define MAX_THREADS 1


//const int STARTING_PREY = 190;
//const int STARTING_PREDATORS = 10;

//float preyMaxSpeedAverage = 100;
//float preyRandomMaxSpeedRange = 4;

//float predatorMaxSpeedAverage = 93;
//float predatorRandomMaxSpeedRange = 4;

float timeRatio = 1.0;

static Uint32* WINDOW_PIXELS = NULL;
static SDL_Surface* WINDOW_SURFACE = NULL;
static SDL_Renderer* WINDOW_RENDERER = NULL;
static SDL_Window* WINDOW = NULL;

static std::mutex mutex;

bool shutdownThreads = false;
#define VELOCITY_PHASE 0
#define POSITION_PHASE 1
int currentPhase = VELOCITY_PHASE;

std::condition_variable threadWakeUpConVar;
std::condition_variable mainThreadWakeUpConVar;

std::mutex threadMutex;
std::vector<std::thread> threads = std::vector<std::thread>();

int workPermits = 0;
int workersFinished = 0;
float currentDeltaTime = 0.0;
int currentBoidIndex = 0;

bool doWork = false;

float seasonTime = 10.0f;
float curSeasonTime = 0.0f;
float curSeason = 1.0f;
float mutationRate = 5.0f;

std::vector<Entity*> entities = std::vector<Entity*>();

class lock
{
private:
	std::mutex * m;
public:
	lock(std::mutex * mutex) : m(mutex)
	{
		m->lock();
	}
	~lock()
	{
		m->unlock();
	}
};

static void InitializeSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		WINDOW = SDL_CreateWindow("Assignment5 ~ Tim Bader & James Asher", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (WINDOW == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			throw SDL_GetError();
		}
		int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;

		//Create renderer for window
		WINDOW_RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED);
		if (WINDOW_RENDERER == NULL)
		{
			printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
			throw SDL_GetError();
		}


		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
			throw(IMG_GetError());
		}


		WINDOW_SURFACE = SDL_GetWindowSurface(WINDOW);

		printf("%d", WINDOW_SURFACE->format->BitsPerPixel);
		//WINDOW_SURFACE->
		SDL_FillRect(WINDOW_SURFACE, NULL, SDL_MapRGB(WINDOW_SURFACE->format, 0x00, 0x00, 0x00));
		WINDOW_PIXELS = (Uint32*)WINDOW_SURFACE->pixels;
	}
}

void workerThreadFunc(int id, std::vector<Entity*>* entities)
{
	printf("Thread %d created\n", id);

	bool work = false;

	while (!shutdownThreads)
	{
		{
			std::unique_lock<std::mutex> locker(threadMutex);
			threadWakeUpConVar.wait(locker, [&]() {return workPermits > 0; });
			lock* l = new lock(&mutex);
			//printf("Thread POSITION %d finished working\n", id);
			workPermits--;
			delete(l);
		}

		//printf("Thread %d got work permit\n", id);

		if (shutdownThreads)
		{
			printf("Thread %d shutting down\n", id);
			break;
		}

		if (currentPhase == VELOCITY_PHASE)
		{
			while (true)
			{
				int currentEntityIdx = 0;

				lock* l = new lock(&mutex);
				currentEntityIdx = currentBoidIndex;
				currentBoidIndex++;
				delete(l);

				if (currentEntityIdx >= entities->size())
					break;

				//printf("Thread POSITION %d idx %d maxIdx %d\n", id, boidIdx, boids->size()-1);

				entities->at(currentEntityIdx)->UpdateVelocity(currentDeltaTime, entities);
			}

			lock* l = new lock(&mutex);
			//printf("Thread POSITION %d finished working\n", id);
			workersFinished++;
			delete(l);
			//mainThreadWakeUpConVar.notify_all();

			if (workersFinished >= MAX_THREADS)
			{
				mainThreadWakeUpConVar.notify_all();
			}
		}
		else if (currentPhase == POSITION_PHASE)
		{
			while (true)
			{
				int entityIdx = 0;

				lock* l = new lock(&mutex);
				entityIdx = currentBoidIndex;
				currentBoidIndex++;
				delete(l);

				if (entityIdx >= entities->size())
					break;

				Entity* e = entities->at(entityIdx);

				//printf("Thread POSITION %d idx %d maxIdx %d\n", id, boidIdx, boids->size()-1);
				e->UpdatePosition(currentDeltaTime);

				float x = e->GetX();
				float y = e->GetY();

				float boarderRange = 0.0f;

				if (y < -boarderRange)
				{
					y = SCREEN_HEIGHT + boarderRange * 2 + y;
					//velocityVector[1] = -velocityVector[1];
				}

				if (y > SCREEN_HEIGHT + boarderRange)
				{
					y = y - (SCREEN_HEIGHT + boarderRange * 2);
					//velocityVector[1] = -velocityVector[1];
				}

				if (x < -boarderRange)
				{
					x = SCREEN_WIDTH + boarderRange * 2 + x;
					//velocityVector[0] = -velocityVector[0];
				}

				if (x > SCREEN_WIDTH + boarderRange)
				{
					x = x - (SCREEN_WIDTH + boarderRange * 2);
					//velocityVector[0] = -velocityVector[0];
				}

				e->SetPosition(x, y);
			}

			lock* l = new lock(&mutex);
			//printf("Thread POSITION %d finished working\n", id);
			workersFinished++;
			delete(l);
			//mainThreadWakeUpConVar.notify_all();

			if (workersFinished >= MAX_THREADS)
			{
				mainThreadWakeUpConVar.notify_all();
			}
		}
	}
}

//void AddNewEntity(float x, float y)
//{
	//for (std::vector<Entity*>::iterator it = entities.begin)
//}



int main(int argc, char* args[])
{

	//////
	//////////////////////
	///		Initialization
	//////////////////////
	//////
	InitializeSDL();
	
	srand(time(NULL));

	int x = 0;

	
	//Add em threads
	for (int i = 0; i < MAX_THREADS; i++)
	{
		threads.push_back(std::thread(workerThreadFunc, i, &entities));
	}

	Uint32 startTicks = SDL_GetTicks();


	for (int i = 0; i < 5; i++)
	{
		Carnivore* carn = new Carnivore(RANDOMFLOAT*SCREEN_WIDTH, RANDOMFLOAT*SCREEN_HEIGHT, RANDOMFLOAT * M_PI * 2,40.0f*RANDOMFLOAT+40.0f,50.0f*RANDOMFLOAT+25.0f,40.0f*RANDOMFLOAT+90.0f,12.0f*RANDOMFLOAT+18.0f );
		carn->SetColor(255, 0, 0, 255);
		entities.push_back(carn);

		//RANDOMFLOAT*100.0f;
		//(60.0f-35.0f)*RANDOMFLOAT+35.0f;
	}

	for (int i = 0; i < 7; i++)
	{
		Omnivore* omni = new Omnivore(RANDOMFLOAT*SCREEN_WIDTH, RANDOMFLOAT*SCREEN_HEIGHT, RANDOMFLOAT * M_PI * 2, 40.0f*RANDOMFLOAT + 40.0f, 50.0f*RANDOMFLOAT + 25.0f, 40.0f*RANDOMFLOAT + 90.0f, 12.0f*RANDOMFLOAT + 18.0f);
		omni->SetColor(255, 255, 0, 255);
		entities.push_back(omni);
	}

	for (int i = 0; i < 15; i++)
	{
		Herbivore* herb = new Herbivore(RANDOMFLOAT*SCREEN_WIDTH, RANDOMFLOAT*SCREEN_HEIGHT, RANDOMFLOAT * M_PI * 2, 40.0f*RANDOMFLOAT + 40.0f, 50.0f*RANDOMFLOAT + 25.0f, 40.0f*RANDOMFLOAT + 90.0f, 12.0f*RANDOMFLOAT + 18.0f);
		herb->SetColor(0, 255, 0, 255);
		entities.push_back(herb);
	}

	for (int i = 0; i < 23; i++)
	{
		Plant* plant = new Plant(RANDOMFLOAT*SCREEN_WIDTH, RANDOMFLOAT*SCREEN_HEIGHT, RANDOMFLOAT * M_PI * 2);
		entities.push_back(plant);
	}

	std::vector<Creature*> deadCreatures = std::vector<Creature*>();
	//float dis = GetAngleMinDifference(1 * M_PI / 180, 335 * M_PI / 180);
	//printf("STUFF: %f\n", (dis * 180 / M_PI));

	//////
	//////////////////////
	///		Game Loop
	//////////////////////
	//////

	bool done = false;
	while (!done)
	{

		float deltaTime = (float)(SDL_GetTicks() - startTicks) / 1000.0 * timeRatio;
		startTicks = SDL_GetTicks();

		//////
		//////////////////////
		///		Input
		//////////////////////
		//////
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case (SDLK_ESCAPE):
					done = true;
					break;
				}

				break;
			
			case SDL_MOUSEBUTTONDOWN:
			{
				int x; int y;
				SDL_GetMouseState(&x, &y);
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					/*Predator* pred = new Predator(x, y);
					//pred->SetMaxSpeed(0.000001);
					boids.push_back(pred);
					pred->SetScale(0.85);
					float speed = predatorMaxSpeedAverage + (RANDOMFLOAT - 1) * predatorRandomMaxSpeedRange;
					if (speed < 0)
						speed = 0;
					pred->SetMaxSpeed(speed);
					pred->SetColor(255, 0, 0, 255);*/
				}
				break;
				case SDL_BUTTON_RIGHT:
				{
					/*Prey* prey = new Prey(x, y);
					prey->SetScale(0.34);
					float speed = preyMaxSpeedAverage + (RANDOMFLOAT - 1) * preyRandomMaxSpeedRange;
					if (speed < 0)
						speed = 0;
					prey->SetMaxSpeed(speed);
					boids.push_back(prey);*/
				}
				break;
				}
			}
			break;

			case SDL_QUIT:
				done = true;
				break;
			}
		}

		//////
		//////////////////////
		///		Updating
		//////////////////////
		//////
		
		currentDeltaTime = deltaTime;


		////////////////////////////
		///		Season and Generation Phase
		////////////////////////////
		curSeasonTime += currentDeltaTime;
		if (curSeasonTime >= seasonTime) {
			curSeason++;
			curSeasonTime = 0.0f;
			int i = 0;
			if (curSeason == 4) {
				curSeason = 1;
				while (true)
				{

					if (i >= entities.size())
						break;
					Creature* creature = dynamic_cast<Creature*>(entities[i]);
					if (creature) {
						creature->SetFitnessScore(100.0f + (creature->GetLifeTime() / 2) + (creature->getHighestEnergy() / 2));
					}
					i++;
				}
				i = 0;
				while (true)
				{
					if (i >= deadCreatures.size())
						break;
					Creature* creature = dynamic_cast<Creature*>(deadCreatures[i]);
					if (creature) {
						creature->SetFitnessScore(50.0f + (creature->GetLifeTime() / 2) + (creature->getHighestEnergy() / 2));
					}
					i++;
				}
				for (int i = 0; i < entities.size(); i++)
				{

					Creature* creature = dynamic_cast<Creature*>(entities[i]);
					if (creature)
						deadCreatures.push_back(creature);
					else
						delete(entities[i]);
						//delete(entities[i]);
					entities.erase(entities.begin() + i);//should resize vector
					i--;
				}
				printf("%d\n", entities.size());
				float carnFitnessTotal = 0.0;
				float omniFitnessTotal = 0.0;
				float herbFitnessTotal = 0.0;
				int k = 0;
				int l = 0;
				int m = 0;
				for (int i = 0; i < deadCreatures.size(); i++)
				{
					Carnivore* carn = dynamic_cast<Carnivore*>(deadCreatures[i]);
					if (carn) {
						k++;
						carnFitnessTotal += carn->GetFitnessScore();
						printf("Fitness Total Carn%d: %f\n", k, carn->GetFitnessScore());
						continue;
					}
					Omnivore* omni = dynamic_cast<Omnivore*>(deadCreatures[i]);
					if (omni) {
						l++;
						omniFitnessTotal += omni->GetFitnessScore();
						printf("Fitness Total Omni%d: %f\n", l, omni->GetFitnessScore());
						continue;
					}
					Herbivore* herb = dynamic_cast<Herbivore*>(deadCreatures[i]);
					if (herb) {
						m++;
						herbFitnessTotal += herb->GetFitnessScore();
						printf("Fitness Total Herb%d: %f\n", m, herb->GetFitnessScore());
						continue;
					}
				}
				for (int i = 0; i < 5; i++)//next gen carnivores
				{
					int parent1idx = 0;
					int parent2idx = 0;
					float par1rnd = RANDOMFLOAT*carnFitnessTotal;
					printf("Parent 1 random value: %f\n", par1rnd);
					float par2rnd = RANDOMFLOAT*carnFitnessTotal;
					float runFitTotal = 0.0;
					for (int j = 0; j < deadCreatures.size(); j++)
					{
						Carnivore* carn = dynamic_cast<Carnivore*>(deadCreatures[j]);
						if (carn) {
							runFitTotal += carn->GetFitnessScore();
							if (runFitTotal >= par1rnd) {
								parent1idx = j;
								break;
							}
						}
					}
					for (int j = 0; j < deadCreatures.size(); j++)
					{
						Carnivore* carn = dynamic_cast<Carnivore*>(deadCreatures[j]);
						if (carn) {
							runFitTotal += carn->GetFitnessScore();
							if (runFitTotal >= par1rnd) {
								parent2idx = j;
								break;
							}
						}
					}
					float newEnergy = 0.0f;
					float newMaxSpeed = 0.0f;
					float newAcceleration = 0.0f;
					float newEnConR = 0.0f;
					if(rand()%2==1)
						newEnergy = deadCreatures[parent1idx]->GetBaseEnergy()+(5.0*RANDOMFLOAT-2.5f);
					else
						newEnergy = deadCreatures[parent2idx]->GetBaseEnergy() + (5.0*RANDOMFLOAT - 2.5f);
					if (rand() % 2 == 1)
						newMaxSpeed = deadCreatures[parent1idx]->GetBaseMaxSpeed() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newMaxSpeed = deadCreatures[parent2idx]->GetBaseMaxSpeed() + (5.0*RANDOMFLOAT - 2.5f);
					if (rand() % 2 == 1)
						newAcceleration = deadCreatures[parent1idx]->GetAcceleration() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newAcceleration = deadCreatures[parent2idx]->GetAcceleration() + (5.0*RANDOMFLOAT - 2.5f);
					if (rand() % 2 == 1)
						newEnConR = deadCreatures[parent1idx]->GetEnergyCosumptionRate() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newEnConR = deadCreatures[parent2idx]->GetEnergyCosumptionRate() + (5.0*RANDOMFLOAT - 2.5f);
					Carnivore* carn = new Carnivore(RANDOMFLOAT*SCREEN_WIDTH, RANDOMFLOAT*SCREEN_HEIGHT, RANDOMFLOAT * M_PI * 2, newEnergy, newMaxSpeed, newAcceleration, newEnConR);
					printf("Carn%d, Parents are %d and %d, Starting Energy %f, MaxSpeed %f, Acceleration %f, Energy Consumption Rate %f\n", i,parent1idx, parent2idx, newEnergy, newMaxSpeed, newAcceleration, newEnConR);
					carn->SetColor(255, 0, 0, 255);
					entities.push_back(carn);

					//RANDOMFLOAT*100.0f;
					//(60.0f-35.0f)*RANDOMFLOAT+35.0f;
				}//spawn next gen carnivores
				for (int i = 0; i < 7; i++)//next gen omnivores
				{
					int parent1idx = 0;
					int parent2idx = 0;
					float par1rnd = RANDOMFLOAT*(omniFitnessTotal);
					float par2rnd = RANDOMFLOAT*(omniFitnessTotal);
					float runFitTotal = 0.0;
					for (int j = 0; j < deadCreatures.size(); j++)
					{
						Omnivore* omni = dynamic_cast<Omnivore*>(deadCreatures[j]);
						if (omni) {
							runFitTotal += omni->GetFitnessScore();
							if (runFitTotal >= par1rnd) {
								parent1idx = j;
								break;
							}
						}
					}
					for (int j = 0; j < deadCreatures.size(); j++)
					{
						Omnivore* omni = dynamic_cast<Omnivore*>(deadCreatures[j]);
						if (omni) {
							runFitTotal += omni->GetFitnessScore();
							if (runFitTotal >= par1rnd) {
								parent2idx = j;
								break;
							}
						}
					}
					float newEnergy = 0.0f;
					float newMaxSpeed = 0.0f;
					float newAcceleration = 0.0f;
					float newEnConR = 0.0f;
					if (rand() % 2 == 1)
						newEnergy = deadCreatures[parent1idx]->GetBaseEnergy() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newEnergy = deadCreatures[parent2idx]->GetBaseEnergy() + (5.0*RANDOMFLOAT - 2.5f);
					if (rand() % 2 == 1)
						newMaxSpeed = deadCreatures[parent1idx]->GetBaseMaxSpeed() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newMaxSpeed = deadCreatures[parent2idx]->GetBaseMaxSpeed() + (5.0*RANDOMFLOAT - 2.5f);
					if (rand() % 2 == 1)
						newAcceleration = deadCreatures[parent1idx]->GetAcceleration() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newAcceleration = deadCreatures[parent2idx]->GetAcceleration() + (5.0*RANDOMFLOAT - 2.5f);
					if (rand() % 2 == 1)
						newEnConR = deadCreatures[parent1idx]->GetEnergyCosumptionRate() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newEnConR = deadCreatures[parent2idx]->GetEnergyCosumptionRate() + (5.0*RANDOMFLOAT - 2.5f);
					Omnivore* omni = new Omnivore(RANDOMFLOAT*SCREEN_WIDTH, RANDOMFLOAT*SCREEN_HEIGHT, RANDOMFLOAT * M_PI * 2, newEnergy, newMaxSpeed, newAcceleration, newEnConR);
					printf("Omni%d, Starting Energy %f, MaxSpeed %f, Acceleration %f, Energy Consumption Rate %f\n", i, newEnergy, newMaxSpeed, newAcceleration, newEnConR);
					omni->SetColor(255, 255, 0, 255);
					entities.push_back(omni);

					//RANDOMFLOAT*100.0f;
					//(60.0f-35.0f)*RANDOMFLOAT+35.0f;
				}//spawn next gen 
				for (int i = 0; i < 15; i++)
				{
					int parent1idx = 0;
					int parent2idx = 0;
					float par1rnd = RANDOMFLOAT*(herbFitnessTotal);
					float par2rnd = RANDOMFLOAT*(herbFitnessTotal);
					float runFitTotal = 0.0;
					for (int j = 0; j < deadCreatures.size(); j++)
					{
						Herbivore* herb = dynamic_cast<Herbivore*>(deadCreatures[j]);
						if (herb) {
							runFitTotal += herb->GetFitnessScore();
							if (runFitTotal >= par1rnd) {
								parent1idx = j;
								break;
							}
						}
					}
					for (int j = 0; j < deadCreatures.size(); j++)
					{
						Herbivore* herb = dynamic_cast<Herbivore*>(deadCreatures[j]);
						if (herb) {
							runFitTotal += herb->GetFitnessScore();
							if (runFitTotal >= par1rnd) {
								parent2idx = j;
								break;
							}
						}
					}
					float newEnergy = 0.0f;
					float newMaxSpeed = 0.0f;
					float newAcceleration = 0.0f;
					float newEnConR = 0.0f;
					if (rand() % 2 == 1)
						newEnergy = deadCreatures[parent1idx]->GetBaseEnergy() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newEnergy = deadCreatures[parent2idx]->GetBaseEnergy() + (5.0*RANDOMFLOAT - 2.5f);
					if (rand() % 2 == 1)
						newMaxSpeed = deadCreatures[parent1idx]->GetBaseMaxSpeed() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newMaxSpeed = deadCreatures[parent2idx]->GetBaseMaxSpeed() + (5.0*RANDOMFLOAT - 2.5f);
					if (rand() % 2 == 1)
						newAcceleration = deadCreatures[parent1idx]->GetAcceleration() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newAcceleration = deadCreatures[parent2idx]->GetAcceleration() + (5.0*RANDOMFLOAT - 2.5f);
					if (rand() % 2 == 1)
						newEnConR = deadCreatures[parent1idx]->GetEnergyCosumptionRate() + (5.0*RANDOMFLOAT - 2.5f);
					else
						newEnConR = deadCreatures[parent2idx]->GetEnergyCosumptionRate() + (5.0*RANDOMFLOAT - 2.5f);
					Herbivore* herb = new Herbivore(RANDOMFLOAT*SCREEN_WIDTH, RANDOMFLOAT*SCREEN_HEIGHT, RANDOMFLOAT * M_PI * 2, newEnergy, newMaxSpeed, newAcceleration, newEnConR);
					printf("Herb%d, Starting Energy %f, MaxSpeed %f, Acceleration %f, Energy Consumption Rate %f\n", i, newEnergy, newMaxSpeed, newAcceleration, newEnConR);
					herb->SetColor(0, 255, 0, 255);
					entities.push_back(herb);

					//RANDOMFLOAT*100.0f;
					//(60.0f-35.0f)*RANDOMFLOAT+35.0f;
				}
				for (int i = 0; i < 23; i++)
				{
					Plant* plant = new Plant(RANDOMFLOAT*SCREEN_WIDTH, RANDOMFLOAT*SCREEN_HEIGHT, RANDOMFLOAT * M_PI * 2);
					entities.push_back(plant);
				}
				continue;
			}
			while (true)
			{

				if (i >= entities.size())
					break;
				Plant* plant = dynamic_cast<Plant*>(entities[i]);
				if (plant) {
					plant->setGrowthRate(10.0f / curSeason);
				}
				i++;
			}

		}
		////////////////////////////
		///		Velocity Phase		~ Boids should NOT move or change any attributes like position and direction
		////////////////////////////
		int currentEntityIdx = 0;
		while (true)
		{

			if (currentEntityIdx >= entities.size())
				break;

			//printf("Thread POSITION %d idx %d maxIdx %d\n", id, boidIdx, boids->size()-1);

			entities.at(currentEntityIdx)->UpdateVelocity(currentDeltaTime, &entities);

			currentEntityIdx++;
		}

		/*currentPhase = VELOCITY_PHASE;
		currentBoidIndex = 0;
		workersFinished = 0;
		workPermits = MAX_THREADS;

		//Need to do busy waiting since workers can finish completly before reaching this point;

		{
			std::unique_lock<std::mutex> locker(threadMutex);
			mainThreadWakeUpConVar.wait(locker, [&]()
			{		
				threadWakeUpConVar.notify_all();
				//printf("WORKERS FINISHED %d\n", workersFinished); 
				return workersFinished >= MAX_THREADS;
			});
		}
		currentBoidIndex = 0;
		workersFinished = 0;*/


		////////////////////////////
		///		Position Phase		~ Boids should NOT sense where other Boids are, as this is the phase where they apply their desired movement
		////////////////////////////

		int entityIndex = 0;
		while (true)
		{
			if (entityIndex >= entities.size())
				break;

			Entity* e = entities.at(entityIndex);
			Creature* creature = dynamic_cast<Creature*>(e);
			if(creature)	creature->AddLifeTime(deltaTime);
			//printf("Thread POSITION %d idx %d maxIdx %d\n", id, boidIdx, boids->size()-1);
			e->UpdatePosition(currentDeltaTime);

			float x = e->GetX();
			float y = e->GetY();

			float boarderRange = 0.0f;

			if (y < -boarderRange)
			{
				y = SCREEN_HEIGHT + boarderRange * 2 + y;
				//velocityVector[1] = -velocityVector[1];
			}

			if (y > SCREEN_HEIGHT + boarderRange)
			{
				y = y - (SCREEN_HEIGHT + boarderRange * 2);
				//velocityVector[1] = -velocityVector[1];
			}

			if (x < -boarderRange)
			{
				x = SCREEN_WIDTH + boarderRange * 2 + x;
				//velocityVector[0] = -velocityVector[0];
			}

			if (x > SCREEN_WIDTH + boarderRange)
			{
				x = x - (SCREEN_WIDTH + boarderRange * 2);
				//velocityVector[0] = -velocityVector[0];
			}

			e->SetPosition(x, y);

			entityIndex++;
		}

		/*currentPhase = POSITION_PHASE;
		currentBoidIndex = 0;
		workersFinished = 0;
		workPermits = MAX_THREADS;

		{
			std::unique_lock<std::mutex> locker(threadMutex);
			mainThreadWakeUpConVar.wait(locker, [&]()
			{
				threadWakeUpConVar.notify_all();
				//printf("WORKERS FINISHED %d\n", workersFinished); 
				return workersFinished >= MAX_THREADS;
			});
		}
		currentBoidIndex = 0;
		workersFinished = 0;*/

		//////
		//////////////////////
		///		Renderering
		//////////////////////
		//////

		SDL_SetRenderDrawColor(WINDOW_RENDERER, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderClear(WINDOW_RENDERER);

		for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it)
		{
			Entity* e = *it;
			int x = e->GetX();
			int y = e->GetY();

			e->DrawAtPosition(WINDOW_RENDERER, x, y);

			///May have wrapping issues in the corners

			float dist = e->GetBaseSize() * e->GetScale() * 2;
			if (x + SCREEN_WIDTH <= SCREEN_WIDTH + dist)
				e->DrawAtPosition(WINDOW_RENDERER, x + SCREEN_WIDTH, y);
			if (x - SCREEN_WIDTH >= -dist)
				e->DrawAtPosition(WINDOW_RENDERER, x - SCREEN_WIDTH, y);
			if (y + SCREEN_HEIGHT <= SCREEN_HEIGHT + dist)
				e->DrawAtPosition(WINDOW_RENDERER, x, y + SCREEN_HEIGHT);
			if (y - SCREEN_HEIGHT >= -dist)
				e->DrawAtPosition(WINDOW_RENDERER, x, y - SCREEN_HEIGHT);

			if (x + SCREEN_WIDTH <= SCREEN_WIDTH + dist && y + SCREEN_HEIGHT <= SCREEN_HEIGHT + dist)
				e->DrawAtPosition(WINDOW_RENDERER, x + SCREEN_WIDTH, y + SCREEN_HEIGHT);
			if (x - SCREEN_WIDTH >= -dist && y + SCREEN_HEIGHT <= SCREEN_HEIGHT + dist)
				e->DrawAtPosition(WINDOW_RENDERER, x - SCREEN_WIDTH, y + SCREEN_HEIGHT);
			if (x + SCREEN_WIDTH <= SCREEN_WIDTH + dist && y - SCREEN_HEIGHT >= -dist)
				e->DrawAtPosition(WINDOW_RENDERER, x + SCREEN_WIDTH, y - SCREEN_HEIGHT);
			if (x - SCREEN_WIDTH >= -dist && y - SCREEN_HEIGHT >= -dist)
				e->DrawAtPosition(WINDOW_RENDERER, x - SCREEN_WIDTH, y - SCREEN_HEIGHT);

		}


		for (int i = 0; i < entities.size(); i++)
		{
			if (entities[i]->GetTagForDeletion())
			{

				Creature* creature = dynamic_cast<Creature*>(entities[i]);
				if (creature != NULL)
				{
					deadCreatures.push_back(creature);
				}
				else
				{
					delete(entities[i]);
				}

				//delete(entities[i]);
				entities.erase(entities.begin() + i);//should resize vector
				
				i--;
			}
		}

		//entities[0]->Get


		printf("fps: %f\n", (1 / deltaTime) * timeRatio);

		SDL_RenderPresent(WINDOW_RENDERER);
	}


	shutdownThreads = true;
	workPermits = MAX_THREADS;
	threadWakeUpConVar.notify_all();

	for (int i = 0; i < MAX_THREADS; i++)
	{
		threads.at(i).join();
	}

	SDL_DestroyWindow(WINDOW);
	SDL_Quit();

	return 0;
}

