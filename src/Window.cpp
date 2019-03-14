#include <iostream>
#include <string>

#include "./../include/Window.h"
#include "./../include/Block.h"
#include "./../include/RushHour.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

Window::Window() {
    this->menus.push_back("create");
    this->menus.push_back("solve");
    this->menus.push_back("hint");
};

void Window::init() {

    //The window we'll be rendering to
    SDL_Window * window = NULL;

    //Initialize SDL_ttf
    if(TTF_Init() == -1) {
        cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError();

        return;
    } else {
        this->font = TTF_OpenFont("./calibri/Calibri.ttf", 24);
        if(this->font == NULL) {
            cout << "TTF_OpenFont: " <<  TTF_GetError() << endl;

            return;
        }
    }

    //Initialize SDL
    if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
    } else {
        //Create window
        window = SDL_CreateWindow("RushHour", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if(window == NULL) {
            cout << "Window could not be created! SDL_Error: " << SDL_GetError();
        } else {
            this->renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

            //Event handler
            SDL_Event e;

            string filepath = "./res/puzzle.txt";

            RushHour rh(filepath);
            rh.solve_forward();

            this->render(rh);

            while(!this->quit) {
                //Handle events on queue
                while(SDL_PollEvent(& e) != 0) {
                    if(e.type == SDL_QUIT) {
                        // user requests quit
                        this->quit = true;
                    } else if(e.type == SDL_KEYDOWN) {
                        // key press
                        switch(e.key.keysym.sym) {
                            case SDLK_ESCAPE : {
                                this->quit = true;

                                break;
                            }
                            case SDLK_LEFT : {
                                rh.cycle(-1);

                                this->render(rh);

                                break;
                            }
                            case SDLK_RIGHT : {
                                rh.cycle(1);

                                this->render(rh);

                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // destroy window
    SDL_DestroyWindow(window);

    // quit SDL subsystems
    SDL_Quit();
};

void Window::render(RushHour & rh) {
    this->clear();

    this->wall(rh.destination->row, rh.destination->col, rh.destination->orientation);
    this->menu();

    int width, height;
    for(unsigned int g = 0; g < rh.blocks.size(); g++) {
        Block * & block = rh.blocks[g];

        width = 1, height = 1;

        if(block->orientation) {
            width = block->length;
        } else {
            height = block->length;
        }

        if(block == rh.target) {
            SDL_SetRenderDrawColor(this->renderer, this->target.r, this->target.g, this->target.b, 255);
        } else {
            SDL_SetRenderDrawColor(this->renderer, this->truck.r, this->truck.g, this->truck.b, 255);
        }

        this->rect(block->row, block->col, width, height);
    }

    SDL_RenderPresent(this->renderer);
};

void Window::rect(int row, int col, int width, int height) {
    SDL_Rect box;

    box.w = width * Window::BLOCK_SIZE - Window::MARGIN_SIZE;
    box.h = height * Window::BLOCK_SIZE - Window::MARGIN_SIZE;

    box.x = (Window::SCREEN_WIDTH - 6 * Window::BLOCK_SIZE) / 2 + col * Window::BLOCK_SIZE + Window::MARGIN_SIZE;
    box.y = (Window::SCREEN_HEIGHT - 6 * Window::BLOCK_SIZE) / 2 + row * Window::BLOCK_SIZE + Window::MARGIN_SIZE;

    SDL_RenderDrawRect(this->renderer, & box);
};

void Window::wall(int row, int col, bool orientation) {
    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    int topLeftCornerX = (Window::SCREEN_WIDTH - 6 * Window::BLOCK_SIZE) / 2;
    int topLeftCornerY = (Window::SCREEN_HEIGHT - 6 * Window::BLOCK_SIZE) / 2;

    int bottomRightCornerX = topLeftCornerX + 6 * Window::BLOCK_SIZE + Window::MARGIN_SIZE;
    int bottomRightCornerY = topLeftCornerY + 6 * Window::BLOCK_SIZE + Window::MARGIN_SIZE;

    SDL_RenderDrawLine(this->renderer, topLeftCornerX, topLeftCornerY, bottomRightCornerX, topLeftCornerY);
    SDL_RenderDrawLine(this->renderer, bottomRightCornerX, topLeftCornerY, bottomRightCornerX, bottomRightCornerY);
    SDL_RenderDrawLine(this->renderer, bottomRightCornerX, bottomRightCornerY, topLeftCornerX, bottomRightCornerY);
    SDL_RenderDrawLine(this->renderer, topLeftCornerX, bottomRightCornerY, topLeftCornerX, topLeftCornerY);

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);

    if(!orientation) {
        int vertical = (row == 0) ?  topLeftCornerY : bottomRightCornerY;

        SDL_RenderDrawLine(this->renderer,
            topLeftCornerX + col * Window::BLOCK_SIZE, vertical,
            topLeftCornerX + (col + 1) * Window::BLOCK_SIZE, vertical
        );
    } else {
        int horizontal = (col == 0) ?  topLeftCornerX : bottomRightCornerX;

        SDL_RenderDrawLine(this->renderer,
            horizontal, topLeftCornerY + row * Window::BLOCK_SIZE,
            horizontal, topLeftCornerY + (row + 1) * Window::BLOCK_SIZE
        );
    }
};

void Window::menu() {
    this->text(Window::SCREEN_WIDTH / 2, 16, "RushHour");

    float offset = Window::SCREEN_WIDTH / (this->menus.size() + 1);
    float acc = offset;
    for(int g = 0; g < this->menus.size(); g++) {
        this->text(acc, Window::SCREEN_HEIGHT - 64, this->menus[g]);

        acc += offset;
    }
};

void Window::text(float x, float y, string text) {
    // set up text color
    SDL_Color color = {255, 255, 255};

    // surface
    SDL_Surface * surface = TTF_RenderText_Solid(this->font, text.c_str(), color);

    // font texture
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

    // create message box
    SDL_Rect message = {x - surface->w / 2, y + surface->h / 2, surface->w, surface->h};

    // copy to main renderer
    SDL_RenderCopy(this->renderer, texture, NULL, & message);

    // destroy font texture
    SDL_DestroyTexture(texture);
};

void Window::clear() {
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
    SDL_RenderClear(this->renderer);
};

void Window::release() {
    this->quit = true;
};