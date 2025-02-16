#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

typedef struct {
    char name[256];
    int carbs;
    int necessaryUnits;
}Meal;

SDL_Window* initializeWindow(void) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    SDL_Window* window = SDL_CreateWindow("Carb Calculator",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    return window;
}

SDL_Renderer* createRenderer(SDL_Window* win) {
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, render_flags);
    return renderer;
}

void selectMeal(int* selectedIndex, bool* running, SDL_Rect* itemRects,
                int itemCount, char* message, size_t messageSize, Meal** items) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            (*selectedIndex) = -1;
            (*running) = false;
        }
        if((*selectedIndex) != -1)
            continue;
        if(event.type == SDL_MOUSEBUTTONDOWN) {
            int mx = event.button.x, my = event.button.y;
            for(int i = 0; i < itemCount; i++) {
                if(mx >= itemRects[i].x && mx < itemRects[i].x + itemRects[i].w &&
                    my >= itemRects[i].y && my < itemRects[i].y + itemRects[i].h) {
                    (*selectedIndex) = i;
                    snprintf(message, messageSize, "%s has %d carbs", items[i]->name, items[i]->carbs);
                    break;
                }
            }
        }
    }
}


int displayMealSelection(int* selectedIndex, SDL_Rect* itemRects, int itemCount, int buttonWidth,
                          int buttonHeight, TTF_Font* font, SDL_Renderer* renderer, Meal** items) {
    
    for(int i = 0; i < itemCount; i++) {
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255); // Button background
        SDL_RenderFillRect(renderer, &itemRects[i]);

        // Highlight selected button
        if(i == (*selectedIndex)) {
            SDL_RenderDrawRect(renderer, &itemRects[i]);
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            SDL_RenderClear(renderer);
            // displayCarbs();
            // askForDose();
            return 1;
        }

        // Render text for menu items
        SDL_Color textColor = { 255, 255, 255, 255 };
        SDL_Surface* surf = TTF_RenderText_Blended(font, items[i]->name, textColor);
        SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);

        if(textTex) {
            int w, h;
            SDL_QueryTexture(textTex, NULL, NULL, &w, &h);
            SDL_Rect dstRect = { itemRects[i].x + (buttonWidth - w) / 2,
                                 itemRects[i].y + (buttonHeight - h) / 2,
                                 w, h };
            SDL_RenderCopy(renderer, textTex, NULL, &dstRect);
            SDL_DestroyTexture(textTex);
        }
    }
    return 0;
}

int singleChoiceMenu(SDL_Window* window, SDL_Renderer* renderer, 
                    TTF_Font* font, Meal** items, int itemCount) {
    int buttonWidth = 200, buttonHeight = 50, spacing = 15;

    // Center the menu vertically
    int totalHeight = (itemCount * buttonHeight) + ((itemCount - 1) * spacing);
    int startY = (WINDOW_HEIGHT - totalHeight) / 2;
    int startX = (WINDOW_WIDTH - buttonWidth) / 2;

    SDL_Rect itemRects[itemCount];
    for(int i = 0; i < itemCount; i++) {
        itemRects[i] = (SDL_Rect){ startX, startY + i * (buttonHeight + spacing), buttonWidth, buttonHeight };
    }

    int selectedIndex = -1;
    bool running = true;
    char message[100] = "";

    while(running) {
        selectMeal(&selectedIndex, &running, itemRects, itemCount, message, sizeof(message), items);
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        displayMealSelection(&selectedIndex, itemRects, itemCount, 
                            buttonWidth, buttonHeight, font, renderer, items);

        // Render selected message
        if(strlen(message) > 0) {
            SDL_Color white = { 255, 255, 255, 255 };
            SDL_Surface* msgSurf = TTF_RenderText_Blended(font, message, white);
            SDL_Texture* msgTex = SDL_CreateTextureFromSurface(renderer, msgSurf);
            SDL_FreeSurface(msgSurf);

            if(msgTex) {
                int w, h;
                SDL_QueryTexture(msgTex, NULL, NULL, &w, &h);
                SDL_Rect msgRect = { (WINDOW_WIDTH - w) / 2, WINDOW_HEIGHT - h * 12, w, h }; // Centered at bottom
                SDL_RenderCopy(renderer, msgTex, NULL, &msgRect);
                SDL_DestroyTexture(msgTex);
            }
        }
        SDL_RenderPresent(renderer);

    }
    return selectedIndex;
}

int main(void) {
    SDL_Window* win = initializeWindow();
    if(!win)
        return 1;

    SDL_Renderer* renderer = createRenderer(win);
    if(!renderer)
        return 1;

    TTF_Init();

    // // Initialize SDL_image for PNG support (adjust flags if needed)
    // IMG_Init(IMG_INIT_PNG);
    
    // // Load the image into a surface
    // SDL_Surface *imageSurface = IMG_Load("path_to_image.png");  // Update the path as needed
    
    // // Create a texture from the surface
    // SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    // SDL_FreeSurface(imageSurface);
    
    // // Query the texture for its width and height
    // int imgWidth, imgHeight;
    // SDL_QueryTexture(imageTexture, NULL, NULL, &imgWidth, &imgHeight);
    
    // // Set the destination rectangle to center the image
    // SDL_Rect destRect;
    // destRect.x = (WINDOW_WIDTH - imgWidth) / 2;
    // destRect.y = (WINDOW_HEIGHT - imgHeight) / 2;
    // destRect.w = imgWidth;
    // destRect.h = imgHeight;
    
    // // Clear the renderer with a black background
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // SDL_RenderClear(renderer);
    
    // // Render the image texture to the destination rectangle
    // SDL_RenderCopy(renderer, imageTexture, NULL, &destRect);
    
    // // Present the renderer so that the image is visible
    // SDL_RenderPresent(renderer);

    TTF_Font* font = TTF_OpenFont("OpenSans.ttf", 24);

    //Define the choices
    Meal* meals[] = { 
        &(Meal){ "Pizza", 300 },
        &(Meal){ "Hamburger", 50 },
        &(Meal){ "Pasta", 100 },
        &(Meal){ "Salad", 25 }
    };
    int numMeals = sizeof(meals) / sizeof(meals[0]);

    int chosen = singleChoiceMenu(win, renderer, font, meals, numMeals);

    if(chosen == -1)
        printf("You closed the window without choosing a meal.\n");
    else
        printf("You chose meal #%d: %s\n", chosen, meals[chosen]->name);

    // SDL_DestroyTexture(imageTexture);
    // IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}
