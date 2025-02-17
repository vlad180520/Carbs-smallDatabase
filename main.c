#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <SDL_image.h>
#include <string.h>

//declared two constants for the dimensions of the window
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480


//declared a struct for keeping the information of a meal
typedef struct {
    char name[256];
    int carbs;
    char image[256];
}Meal;


//function for initializing and creating a window
SDL_Window* initializeWindow(void) {
    //initializing video and timer subsystems for window management
    //and timing functions
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    //declared a window by calling SDL_CreateWindow
    //the constants will center the window
    //0 is for stating there are no flags present (fullscreen etc.)
    SDL_Window* window = SDL_CreateWindow("Carb Calculator",
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    //returning the window in case it cannot be created (the value is NULL)
    return window;
}

//function for rendering the window (displaying the visual content of the window)
SDL_Renderer* createRenderer(SDL_Window* win) {

    //declaring a 32-bit flag for optimizing rendering performance and visual quality
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    //
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, render_flags);
    return renderer;
}

void selectMeal(int* selectedIndex, bool* running, SDL_Rect* itemRects,
                int itemCount, char* message, size_t messageSize, Meal** items) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            *selectedIndex = -1;
            *running = false;
        }
        if(*selectedIndex != -1)
            break;
        if(event.type == SDL_MOUSEBUTTONDOWN) {
            int mx = event.button.x, my = event.button.y;
            for(int i = 0; i < itemCount; i++) {
                if(mx >= itemRects[i].x && mx < itemRects[i].x + itemRects[i].w &&
                    my >= itemRects[i].y && my < itemRects[i].y + itemRects[i].h) {
                    *selectedIndex = i;
                    *running = false;  // Exit menu loop once a selection is made.
                    break;
                }
            }
        }
    }
}

int displayMealSelection(int* selectedIndex, SDL_Rect* itemRects, 
                         int itemCount, int buttonWidth, int buttonHeight, 
                         TTF_Font* font, SDL_Renderer* renderer, Meal** items) {
    for(int i = 0; i < itemCount; i++) {
        // Draw button background
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderFillRect(renderer, &itemRects[i]);
        
        // Highlight the selected button
        if(i == (*selectedIndex)) {
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            SDL_RenderDrawRect(renderer, &itemRects[i]);
        }
        
        // Render the meal name
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

void displayImage(SDL_Renderer* renderer, Meal** meals, int chosen, TTF_Font *font) {
// Clear the screen before displaying the message and image.
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        
        // Create the message texture.
        char displayMsg[256];
        snprintf(displayMsg, sizeof(displayMsg), "%s has %d carbs", meals[chosen]->name, meals[chosen]->carbs);
        SDL_Color textColor = { 255, 255, 255, 255 };
        SDL_Surface* msgSurface = TTF_RenderText_Blended(font, displayMsg, textColor);
        SDL_Texture* msgTexture = SDL_CreateTextureFromSurface(renderer, msgSurface);
        SDL_FreeSurface(msgSurface);
        int msgW, msgH;
        SDL_QueryTexture(msgTexture, NULL, NULL, &msgW, &msgH);
        SDL_Rect msgRect = { (WINDOW_WIDTH - msgW) / 2, WINDOW_HEIGHT - msgH - 20, msgW, msgH };
        
        // Load the meal image.
        SDL_Texture* mealTexture = IMG_LoadTexture(renderer, meals[chosen]->image);
        if(!mealTexture) {
            printf("Failed to load image: %s\n", IMG_GetError());
        } else {
            // Define the destination rectangle for the image.
            SDL_Rect imgRect = { 220, 100, 200, 200 };
            SDL_RenderCopy(renderer, mealTexture, NULL, &imgRect);
            SDL_RenderCopy(renderer, msgTexture, NULL, &msgRect);
            SDL_RenderPresent(renderer);
        }
        SDL_DestroyTexture(msgTexture);
        
        // Wait until the user closes the window (only SDL_QUIT events will exit)
        bool exitLoop = false;
        SDL_Event event;
        while(!exitLoop) {
            while(SDL_PollEvent(&event)) {
                if(event.type == SDL_QUIT) {
                    exitLoop = true;
                }
            }
            SDL_Delay(10);
        }
        
        if(mealTexture) {
            SDL_DestroyTexture(mealTexture);
        }
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
    // Menu loop: continue until a meal is selected or the window is closed.
    while(running) {
        selectMeal(&selectedIndex, &running, itemRects, itemCount, message, sizeof(message), items);
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        displayMealSelection(&selectedIndex, itemRects, itemCount, buttonWidth, buttonHeight, font, renderer, items);
        
        // Render the message if available.
        if(strlen(message) > 0) {
            SDL_Color white = { 255, 255, 255, 255 };
            SDL_Surface* msgSurf = TTF_RenderText_Blended(font, message, white);
            SDL_Texture* msgTex = SDL_CreateTextureFromSurface(renderer, msgSurf);
            SDL_FreeSurface(msgSurf);
            if (msgTex) {
                int w, h;
                SDL_QueryTexture(msgTex, NULL, NULL, &w, &h);
                SDL_Rect msgRect = { (WINDOW_WIDTH - w) / 2, WINDOW_HEIGHT - h - 20, w, h };
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
    if (!win)
        return 1;
    
    SDL_Renderer* renderer = createRenderer(win);
    if (!renderer)
        return 1;
    
    if (TTF_Init() == -1)
        return 1;
    TTF_Font* font = TTF_OpenFont("OpenSans.ttf", 24);
    if (!font)
        return 1;
    
    IMG_Init(IMG_INIT_PNG);
    
    // Define the meal choices (ensure each meal has a valid image path).
    Meal* meals[] = { 
        &(Meal){ "Pizza", 300, "pizza.png" },
        &(Meal){ "Hamburger", 50, "hamburger.png" },
        &(Meal){ "Pasta", 100, "pasta.png" },
        &(Meal){ "Salad", 25, "salad.png" },
        &(Meal){ "Salad", 25, "salad.png" },
        &(Meal){ "Salad", 25, "salad.png" },
        &(Meal){ "Salad", 25, "salad.png" },
        &(Meal){ "Salad", 25, "salad.png" },
        &(Meal){ "Salad", 25, "salad.png" }
    };
    int numMeals = sizeof(meals) / sizeof(meals[0]);
    
    int chosen = singleChoiceMenu(win, renderer, font, meals, numMeals);
    if (chosen == -1) {
        printf("You closed the window without choosing a meal.\n");
    } else {
        printf("You chose: %s\n", meals[chosen]->name);
        displayImage(renderer, meals, chosen, font);
    }
    
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
