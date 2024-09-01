export module VanityUI;

import Page;
import Elements;
import <tuple>;
#include <SDL_image.h>
#include <iostream>;
#include "SDL_ttf.h"

export class Vanity
{
public:
	// Library setup methods 
	static void Setup(int viewWidth, int viewHeight, SDL_Renderer* targetRenderer = nullptr);
	static void SetRenderTarget(SDL_Renderer* renderer);

	// Element creation methods 
	static Label* CreateLabel(std::string text = "Label", int x = 0, int y = 0, SDL_Color color = { 0, 0, 0, 255 }, int fontSize = 15, std::string fontPath = "fonts/CascadiaCode.ttf");
	static Button* CreateButton(std::string label = "Button", int x = 0, int y = 0, int width = 0, int height = 0, int fontSize = 15, std::string fontPath = "fonts/CascadiaCode.ttf");
	static Textbox* CreateTextbox(std::string placeholder = "Textbox", int x = 0, int y = 0, int width = 0, int height = 0, int fontSize = 15, int limit = NULL, std::string fontPath = "fonts/CascadiaCode.ttf");
	static Checkbox* CreateCheckbox(int x = 0, int y = 0, int size = 20, bool defaultState = false);
	static Image* CreateImage(std::string imagePath, int x = 0, int y = 0, int width = 100, int height = 100);
	static Slider* CreateSlider(int x = 0, int y = 0, int width = 120, int height = 20, int thumbWidth = 20, int thumbHeight = 20);
	static Division* CreateDivision(int x = 0, int y = 0, int width = 100, int height = 100);

	// Page related methods 
	static Page* CreatePage();
	static void DisplayPage(Page* page);

	// Library utility methods 
	static void RenderLabel(std::string text, int x, int y, SDL_Color color, TTF_Font* font, int fontSize = 12);
	static void DrawCircle(int32_t centreX, int32_t centreY, int32_t radius);
	static void Render();
	static void Rerender(); // TODO: Only render new frame texture when rerender bool is true!
	static SDL_Texture* LoadImage(std::string imagePath);
	static bool IsRunning();
	static void Prepare();
	static SDL_Renderer* CreateRenderingContext(std::string title);
	static void EnableVsync();
	static void DisableVsync();
	static int GetViewportWidth();
	static int GetViewportHeight();
	static void SetActiveTextbox(Textbox* textbox);

private:
	// General library data 
	static SDL_Renderer* targetRenderer;
	static bool leftMousePressed;
	static bool leftMouseHeld;
	static bool rerender;
	static bool isRunning;
	static bool vsync;
	static int viewportWidth, viewportHeight, mX, mY;
	static SDL_Texture* snapshotFrame;

	// PAGES - Related data 
	static Page* currentPage;

	// TEXTBOX - Related data  
	static Textbox* activeTextbox;
	static Uint32 textboxCursorDelta, delta;
	static char lastPressedKey;
	static bool drawTextBoxCursor, capsLockEnabled;

	// Private methods  
	static void UpdateMousePosision();
	static bool InheritStateFromParent(Division* parent, int& elementX, int& elementY, bool elementDisplayState);
	static void RenderBackgroundImage(SDL_Texture* image, int width, int height, int x, int y);
	static void prepareNewSnapshotFrame();
	static void finalizeNewSnapshotFrame();
	static TTF_Font* OpenFont(std::string fontUrl, int size);
	static bool OnMouseHover(int x, int y, int width, int height);
	static void UpdateMouseButtonState();
	static bool ValidKey(int key);
	static void CaptureInputText();
	static void RenderButton(Button* button);
	static void RenderTextbox(Textbox* textbox);
	static void RenderLabel(Label* label);
	static void RenderCheckbox(Checkbox* checkbox);
	static void RenderImage(Image* image);
	static void RenderSlider(Slider* slider);
	static void RenderDivision(Division* division);
	static void RenderBorder(int x, int y, int width, int height, BorderThickness borderThickness, BorderColors borderColors);
};

// IvoryUI.cpp : Implements the methods for the library.

SDL_Renderer* Vanity::targetRenderer = nullptr;
Uint32 Vanity::delta;
Uint32 Vanity::textboxCursorDelta;
Textbox* Vanity::activeTextbox = nullptr;
char Vanity::lastPressedKey;
bool Vanity::leftMousePressed = false;
bool Vanity::leftMouseHeld = false;
bool Vanity::isRunning = false;
bool Vanity::drawTextBoxCursor = true;
bool Vanity::capsLockEnabled = false;
bool Vanity::rerender = false;
bool Vanity::vsync = true;
int Vanity::viewportWidth = 0;
int Vanity::viewportHeight = 0;
int Vanity::mX = 0;
int Vanity::mY = 0;
SDL_Texture* Vanity::snapshotFrame = nullptr;
Page* Vanity::currentPage = nullptr;

// LIBRARY SETUP METHODS

void Vanity::Setup(int viewWidth, int viewHeight, SDL_Renderer* renderer) {
	// Initializes the SDL2 library
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		char t[] = "Initialization error";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, t, SDL_GetError(), nullptr);
		exit(0);
	}

	// Initializes the SDL font library
	if (TTF_Init() < 0) {
		char t[] = "TTF error";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, t, TTF_GetError(), nullptr);
		exit(0);
	}

	// Initializes the SDL image library
	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP) < 0) {
		char t[] = "IMG error";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, t, IMG_GetError(), nullptr);
		exit(0);
	}

	targetRenderer = renderer;
	isRunning = true;

	// To be used for creating frame snapshots
	viewportWidth = viewWidth;
	viewportHeight = viewHeight;

	delta = SDL_GetTicks(); // Init milliseconds to be used for textbox input 
	textboxCursorDelta = SDL_GetTicks(); // Init milliseconds to be used for textbox cursor blinking 
}

void Vanity::SetRenderTarget(SDL_Renderer* renderer) {
	targetRenderer = renderer;
}

// ELEMENT CREATION METHODS 

Label* Vanity::CreateLabel(std::string text, int x, int y, SDL_Color color, int fontSize, std::string fontPath) {
	return new Label(text, x, y, color, fontSize, fontPath);
}

Button* Vanity::CreateButton(std::string label, int x, int y, int width, int height, int fontSize, std::string fontPath) {
	auto btn = new Button(label, width, height, x, y, fontSize, fontPath);
	if (!width && !height) {
		int widthRef = 0;
		int heightRef = 0;
		ComputeDimensionsOfText(btn->GetFont(), label.length(), widthRef, heightRef);
		btn->SetDimensions(widthRef, heightRef);
	}
	return btn;
}

Textbox* Vanity::CreateTextbox(std::string placeholder, int x, int y, int width, int height, int fontSize, int limit, std::string fontPath) {
	auto textbox = new Textbox(placeholder, width, height, x, y, fontSize, limit, fontPath);
	if (!width && !height) {
		int widthRef = 0;
		int heightRef = 0;
		ComputeDimensionsOfText(textbox->GetFont(), placeholder.length(), widthRef, heightRef);
		textbox->SetDimensions(widthRef, heightRef);
	}
	return textbox;
}

Checkbox* Vanity::CreateCheckbox(int x, int y, int size, bool defaultState) {
	return new Checkbox(x, y, size, defaultState);
}

Image* Vanity::CreateImage(std::string imagePath, int x, int y, int width, int height) {
	return new Image(imagePath, x, y, width, height);
}

Slider* Vanity::CreateSlider(int x, int y, int width, int height, int thumbWidth, int thumbHeight) {
	return new Slider(x, y, width, height, thumbWidth, thumbHeight);
}

Division* Vanity::CreateDivision(int x, int y, int width, int height) {
	return new Division(x, y, width, height);
}

// ELEMENT RENDERING METHODS 

void Vanity::RenderLabel(Label* label) {
	int x = label->GetX();
	int y = label->GetY();

	bool display = InheritStateFromParent(label->GetParent(), x, y, label->GetDisplayState());

	if (!display) return;

	TTF_Font* font = label->GetFont();
	std::string text = label->GetText();
	int textWidth = 0;
	int textHeight = 0;

	TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

	bool mHover = OnMouseHover(x, y, textWidth, textHeight);

	if (mHover && leftMousePressed)
		label->SetClickedState(true);

	RenderLabel(text, x, y, label->GetColor(), font, label->GetFontSize());
}

void Vanity::RenderButton(Button* button) {
	int x = button->GetX();
	int y = button->GetY();

	Division* parentDiv = button->GetParent();

	//std::cout << "X: " << x << ", Y: " << y << std::endl; 

	bool display = InheritStateFromParent(parentDiv, x, y, button->GetDisplayState());

	if (!display) return;

	// Get necessary data from current object
	Padding padding = button->GetPadding();
	BorderThickness borderThickness = button->GetBorderThickness();
	int width = button->GetWidth() + padding.left + padding.right;
	int height = button->GetHeight() + padding.top + padding.bottom;
	int computedWidth = button->GetComputedWidth();
	int computedHeight = button->GetComputedHeight();
	SDL_Color color = button->GetColor();
	SDL_Color hoverColor = button->GetHoverColor();
	TTF_Font* font = button->GetFont();
	std::string label = button->GetLabel();

	if (button->GetAnimationState()) {
		// If element is currently being animated 
		button->CalculateNextAnimationStep(x, y, *button);
	}

	// Render background image
	if (button->GetBackgroundImageDisplayState())
		Vanity::RenderBackgroundImage(button->GetBackgroundImage(), width, height, x, y);

	// Create button rectangle data
	SDL_Rect rect;
	rect.w = width;
	rect.h = height;
	rect.x = x + borderThickness.left;
	rect.y = y + borderThickness.top;

	bool mHover = OnMouseHover(x, y, computedWidth, computedHeight);

	// If mouse doesn't hover over button, default idle state
	SDL_SetRenderDrawColor(targetRenderer, color.r, color.g, color.b, color.a);
	button->SetClickedState(false);

	// If mouse hovers over button and activates
	if (mHover && leftMousePressed) {
		button->SetClickedState(true);
		SDL_SetRenderDrawColor(targetRenderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a - 75);
		activeTextbox = nullptr;
	}
	// If mouse hovers over
	else if (mHover) {
		if (!leftMouseHeld)
			SDL_SetRenderDrawColor(targetRenderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
		else
			SDL_SetRenderDrawColor(targetRenderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a - 75);
	}

	// Draw button rectangle
	SDL_RenderFillRect(targetRenderer, &rect);

	// Draw button border 
	RenderBorder(x, y, width, height, borderThickness, button->GetBorderColors());

	int textWidth = 0, textHeight = 0;
	TTF_SizeText(font, label.c_str(), &textWidth, &textHeight);

	width = width - padding.left - padding.right;
	height = height - padding.top - padding.bottom;

	// Display button label
	RenderLabel(label, x + borderThickness.left + padding.left + width / 2 - textWidth / 2, y + borderThickness.top + padding.top + height / 2 - textHeight / 2, button->GetFontColor(), font, button->GetFontSize());
}

void Vanity::RenderTextbox(Textbox* textbox) {
	int x = textbox->GetX();
	int y = textbox->GetY();

	bool display = InheritStateFromParent(textbox->GetParent(), x, y, textbox->GetDisplayState());

	if (!display) return;

	// Get necessary data from current object
	Padding padding = textbox->GetPadding();
	BorderThickness borderThickness = textbox->GetBorderThickness();
	int width = textbox->GetWidth() + padding.left + padding.right;
	int height = textbox->GetHeight() + padding.top + padding.bottom;
	int computedWidth = textbox->GetComputedWidth();
	int computedHeight = textbox->GetComputedHeight();
	SDL_Color color = textbox->GetColor();
	SDL_Color fontColor = textbox->GetFontColor();
	SDL_Color hoverColor = textbox->GetHoverColor();
	TTF_Font* font = textbox->GetFont();
	int fontSize = textbox->GetFontSize();
	std::string value = textbox->GetValue();
	std::string placeholder = textbox->GetPlaceholder();

	// Render background image
	if (textbox->GetBackgroundImageDisplayState())
		Vanity::RenderBackgroundImage(textbox->GetBackgroundImage(), width, height, x, y);

	// Create textbox rectangle data
	SDL_Rect rect;
	rect.w = width;
	rect.h = height;
	rect.x = x + borderThickness.top;
	rect.y = y + borderThickness.left;

	bool mHover = OnMouseHover(x, y, computedWidth, computedHeight);

	// If mouse doesn't hover over textbox, default idle state
	SDL_SetRenderDrawColor(targetRenderer, color.r, color.g, color.b, color.a);

	// If mouse hovers over textbox and activates
	if (mHover && leftMousePressed) {
		SDL_SetRenderDrawColor(targetRenderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
		activeTextbox = textbox;
		textbox->SetClickedState(true);
	}
	// If mouse hovers over
	else if (mHover) {
		SDL_SetRenderDrawColor(targetRenderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
	}

	// Draw textbox rectangle
	SDL_RenderFillRect(targetRenderer, &rect);

	// Draw textbox border 
	RenderBorder(x, y, width, height, borderThickness, textbox->GetBorderColors());

	int textWidth = 0, textHeight = 0;
	int lblX, lblY;

	width = width - padding.left - padding.right;
	height = height - padding.top - padding.bottom;

	// If no value, show placeholder
	if (value.empty()) {
		TTF_SizeText(font, placeholder.c_str(), &textWidth, &textHeight);
		lblX = x + borderThickness.left + padding.left + width / 2 - textWidth / 2;
		lblY = y + borderThickness.top + padding.top + height / 2 - textHeight / 2;

		// Display textbox placeholder text
		SDL_Color placeholderColor = fontColor;
		placeholderColor.a = 175;
		RenderLabel(placeholder, lblX, lblY, placeholderColor, font, fontSize);
	}
	// If textbox has a user entered value, show that value in textbox
	else {
		// Reduce shown text if text width greater than textbox width
		std::string reducedText = value;
		TTF_SizeText(font, reducedText.c_str(), &textWidth, &textHeight);
		if (textWidth > width) {
			int minx, maxx;
			TTF_GlyphMetrics(font, 65, &minx, &maxx, nullptr, nullptr, nullptr);
			int widthPerChar = maxx - minx;
			int supportedChars = width / widthPerChar;
			if (reducedText.length() > supportedChars) {
				// Shown text need to be truncated to fit 
				int start = value.length() - supportedChars;
				reducedText = value.substr(start, start - value.length());
			}
		}

		TTF_SizeText(font, reducedText.c_str(), &textWidth, &textHeight);
		lblX = x + borderThickness.left + padding.left + width / 2 - textWidth / 2;
		lblY = y + borderThickness.top + padding.top + height / 2 - textHeight / 2;

		// Display textbox label
		RenderLabel(reducedText, lblX, lblY, fontColor, font, fontSize);
	}

	// If there's an active textbox, toggle textbox cursor every 600 millisecond
	if (activeTextbox) {
		Uint32 now = SDL_GetTicks();
		Uint32 cursorDelta = now - textboxCursorDelta;
		if (cursorDelta > 600) {
			drawTextBoxCursor = !drawTextBoxCursor;
			textboxCursorDelta = now;
		}
	}

	// If active textbox is the current textbox and timing is right, draw cursor on textbox
	if (activeTextbox == textbox && drawTextBoxCursor)
	{
		rect;
		rect.w = 2;
		rect.h = fontSize;
		rect.x = lblX + textWidth;
		rect.y = y + borderThickness.top + padding.top + height / 2 - fontSize / 2;
		SDL_SetRenderDrawColor(targetRenderer, 255, 255, 255, 255);
		SDL_RenderFillRect(targetRenderer, &rect);
	}
	CaptureInputText();
}

void Vanity::RenderCheckbox(Checkbox* checkbox) {	// TODO: Draw v-mark inside checkbox (if selected) to show its state
	int x = checkbox->GetX();
	int y = checkbox->GetY();

	bool display = InheritStateFromParent(checkbox->GetParent(), x, y, checkbox->GetDisplayState());

	if (!display) return;

	// Get necessary data from current object
	int size = checkbox->GetSize();
	int computedSize = checkbox->GetComputedWidth(); // Use width here since width and height is the same
	bool checked = checkbox->IsChecked();
	BorderThickness borderThickness = checkbox->GetBorderThickness();
	SDL_Color color = checkbox->GetColor();

	// Render background image
	if (checkbox->GetBackgroundImageDisplayState())
		Vanity::RenderBackgroundImage(checkbox->GetBackgroundImage(), size, size, x, y);

	// Create checkbox rectangle data
	SDL_Rect rect;
	rect.w = size;
	rect.h = size;
	rect.x = x + borderThickness.left;
	rect.y = y + borderThickness.top;

	bool mHover = OnMouseHover(x, y, computedSize, computedSize);

	// If mouse doesn't hover over checkbox, default idle state
	SDL_SetRenderDrawColor(targetRenderer, color.r, color.g, color.b, color.a);

	// If mouse hovers over button and activates
	if (mHover && leftMousePressed && !checkbox->GetClickedLastFrame()) {
		SDL_Color hoverColor = checkbox->GetHoverColor();
		SDL_SetRenderDrawColor(targetRenderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a - 75);
		checkbox->SetClickedState(true);
		checkbox->SetState(!checked);
		checkbox->SetClickedLastFrame(true);
		activeTextbox = nullptr;
	}
	// If mouse hovers over
	else if (mHover) {
		checkbox->SetClickedLastFrame(false);
		SDL_Color hoverColor = checkbox->GetHoverColor();
		if (!leftMouseHeld)
			SDL_SetRenderDrawColor(targetRenderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
		else
			SDL_SetRenderDrawColor(targetRenderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a - 75);
	}

	// Draw checkbox rectangle
	SDL_RenderFillRect(targetRenderer, &rect);

	// Draw checkbox border 
	RenderBorder(x, y, size, size, checkbox->GetBorderThickness(), checkbox->GetBorderColors());

	// Draw checkmark if checked 
	if (checked) {
		rect.w = size - size * 0.6f;
		rect.h = size - size * 0.6f;
		rect.x = x + borderThickness.left + size * 0.6f / 2;
		rect.y = y + borderThickness.top + size * 0.6f / 2;

		SDL_Color checkmarkColor = checkbox->GetCheckmarkColor();
		SDL_SetRenderDrawColor(targetRenderer, checkmarkColor.r, checkmarkColor.g, checkmarkColor.b, checkmarkColor.a);
		SDL_RenderFillRect(targetRenderer, &rect);
	}
}

void Vanity::RenderLabel(std::string text, int x, int y, SDL_Color color, TTF_Font* font, int fontSize) {

	// Create surface to render text on
	SDL_Surface* surfaceMessage =
		TTF_RenderText_Blended(font, text.c_str(), color);

	// Convert to texture
	SDL_Texture* message = SDL_CreateTextureFromSurface(targetRenderer, surfaceMessage);

	// Create a rectangle/shape of the message texture
	SDL_Rect message_rect;
	message_rect.x = x;
	message_rect.y = y;
	message_rect.w = surfaceMessage->w;
	message_rect.h = surfaceMessage->h;

	SDL_RenderCopy(targetRenderer, message, nullptr, &message_rect);

	// Frees resources 
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);
}

void Vanity::RenderImage(Image* image) {
	int x = image->GetX();
	int y = image->GetY();

	bool display = InheritStateFromParent(image->GetParent(), x, y, image->GetDisplayState());

	if (!display) return;

	int width = image->GetWidth();
	int height = image->GetHeight();

	// Create image rectangle data
	SDL_Rect rect;
	rect.w = width;
	rect.h = height;
	rect.x = x;
	rect.y = y;

	bool mHover = OnMouseHover(x, y, width, height);

	if (mHover && leftMousePressed)
		image->SetClickedState(true);

	SDL_RenderCopy(targetRenderer, image->GetImage(), nullptr, &rect);

	// Draw image border 
	RenderBorder(x, y, image->GetWidth(), image->GetHeight(), image->GetBorderThickness(), image->GetBorderColors());
}

void Vanity::RenderSlider(Slider* slider) {
	int x = slider->GetX();
	int y = slider->GetY();

	bool display = InheritStateFromParent(slider->GetParent(), x, y, slider->GetDisplayState());

	if (!display) return;

	int width = slider->GetWidth();
	int height = slider->GetHeight();
	int thumbHeight = slider->GetThumbHeight();

	// Render background image
	if (slider->GetBackgroundImageDisplayState())
		Vanity::RenderBackgroundImage(slider->GetBackgroundImage(), width, height, x, y);

	// Create slider rectangle data
	SDL_Rect sliderRect;
	sliderRect.w = width;
	sliderRect.h = height;
	sliderRect.x = x;
	sliderRect.y = y;

	// Create slider thumb rectangle data 
	SDL_Rect thumbRect;
	thumbRect.w = width;
	thumbRect.h = height;
	thumbRect.x = x;
	thumbRect.y = y;

	bool baseHover = OnMouseHover(x, y, width, height);
	//bool thumbHover = onMouseHover()	// TODO: See if mouse also hovers over thumb

	SDL_Color sliderColor = slider->GetColor();
	SDL_Color thumbColor = slider->GetThumbColor();

	// Draw slider border
	RenderBorder(x, y, width, height, slider->GetBorderThickness(), slider->GetBorderColors());

	// If mouse doesn't hover over slider, default idle state
	SDL_SetRenderDrawColor(targetRenderer, sliderColor.r, sliderColor.g, sliderColor.b, sliderColor.a);

	if (!slider->GetTouched())
		thumbRect.x = slider->GetThumbPosision() + x;
	else
		thumbRect.x = slider->GetThumbPosision();

	thumbRect.w = slider->GetThumbWidth();
	thumbRect.h = thumbHeight;
	thumbRect.y = thumbRect.y + height / 2 - thumbHeight / 2;

	// Check if slider has been clicked and set state accordingly
	if (baseHover && leftMousePressed)
		slider->SetClickedState(true);

	// If mouse hovers over slider and activates
	if (baseHover && leftMouseHeld) {
		slider->SetTouched(true);

		int mx;
		Uint32 mb = SDL_GetMouseState(&mx, nullptr);

		// Recalculate posision of thumb if mouse is within slider bounds
		if (mx > x && mx < x + width) {
			thumbRect.x = mx - thumbRect.w / 2;
			slider->SetThumbPosision(thumbRect.x);

			// Calculate value and set it 
			float baseline = (float)thumbRect.x + (float)thumbRect.w / 2 - x;
			slider->SetValue((int)(baseline / width * 100));
		}
		activeTextbox = nullptr;
		SDL_SetRenderDrawColor(targetRenderer, sliderColor.r, sliderColor.g, sliderColor.b, sliderColor.a - 25);
	}
	else if (baseHover) {	// If mouse hovers over
		SDL_Color hoverColor = slider->GetHoverColor();
		if (!leftMouseHeld)
			SDL_SetRenderDrawColor(targetRenderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
		else
			SDL_SetRenderDrawColor(targetRenderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a - 75);
	}

	SDL_RenderFillRect(targetRenderer, &sliderRect);
	SDL_SetRenderDrawColor(targetRenderer, thumbColor.r, thumbColor.g, thumbColor.b, thumbColor.a);
	SDL_RenderFillRect(targetRenderer, &thumbRect);
}

void Vanity::RenderDivision(Division* division) {
	int x = division->GetX();
	int y = division->GetY();

	bool display = InheritStateFromParent(division->GetParent(), x, y, division->GetDisplayState());

	if (!display) return;

	Padding padding = division->GetPadding();
	int width = division->GetComputedWidth();
	int height = division->GetComputedHeight();

	bool mHover = OnMouseHover(x, y, width, height);

	// Check if division has been clicked and set state accordingly
	if (mHover && leftMousePressed)
		division->SetClickedState(true);

	// Render background image
	if (division->GetBackgroundImageDisplayState())
		Vanity::RenderBackgroundImage(division->GetBackgroundImage(), width, height, x, y);

	// Create image rectangle data
	SDL_Rect rect;
	rect.w = width;
	rect.h = height;
	rect.x = x;
	rect.y = y;

	auto color = division->GetColor();
	SDL_SetRenderDrawColor(targetRenderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(targetRenderer, &rect);

	// Draw division border 
	RenderBorder(x, y, division->GetWidth(), division->GetHeight(), division->GetBorderThickness(), division->GetBorderColors());
}

void Vanity::RenderBorder(int x, int y, int width, int height, BorderThickness borderThickness, BorderColors borderColors) {
	SDL_Rect border;
	border.w = width;

	// Draw top border if set
	if (borderThickness.top) {
		border.x = x + borderThickness.left;
		border.y = y;
		border.h = borderThickness.top;
		SDL_Color color = borderColors.top;
		SDL_SetRenderDrawColor(targetRenderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(targetRenderer, &border);
	}

	// Draw bottom border if set
	if (borderThickness.bottom) {
		border.x = x + borderThickness.left;
		border.y = y + height + borderThickness.top;
		border.h = borderThickness.bottom;
		SDL_Color color = borderColors.bottom;
		SDL_SetRenderDrawColor(targetRenderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(targetRenderer, &border);
	}

	// Draw right border if set
	if (borderThickness.right) {
		border.x = x + width + borderThickness.left;
		border.y = y;
		border.w = borderThickness.right;
		border.h = height + borderThickness.bottom + borderThickness.top;
		SDL_Color color = borderColors.right;
		SDL_SetRenderDrawColor(targetRenderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(targetRenderer, &border);
	}

	// Draw left border if set
	if (borderThickness.left) {
		border.x = x;
		border.y = y;
		border.w = borderThickness.left;
		border.h = height + borderThickness.bottom + borderThickness.top;
		SDL_Color color = borderColors.left;
		SDL_SetRenderDrawColor(targetRenderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(targetRenderer, &border);
	}
}

void Vanity::Render() {
	UpdateMouseButtonState();
	UpdateMousePosision();

	// Render elements
	auto elements = currentPage->GetElements();
	for (int i = 0; i < elements->size(); i++) {
		if (Division* current = dynamic_cast<Division*>((*elements)[i])) {
			RenderDivision(current);
		}
		else if (Button* current = dynamic_cast<Button*>((*elements)[i])) {
			RenderButton(current);
		}
		else if (Textbox* current = dynamic_cast<Textbox*>((*elements)[i])) {
			RenderTextbox(current);
		}
		else if (Checkbox* current = dynamic_cast<Checkbox*>((*elements)[i])) {
			RenderCheckbox(current);
		}
		else if (Slider* current = dynamic_cast<Slider*>((*elements)[i])) {
			RenderSlider(current);
		}
		else if (Image* current = dynamic_cast<Image*>((*elements)[i])) {
			RenderImage(current);
		}
		else if (Label* current = dynamic_cast<Label*>((*elements)[i])) {
			RenderLabel(current);
		}
	}
	delete elements;
	SDL_RenderPresent(targetRenderer);
}

// PAGES 

Page* Vanity::CreatePage() {
	return new Page();
}

void Vanity::DisplayPage(Page* page) {
	currentPage = page;
	rerender = true;
}

// UTILITY METHODS 

void Vanity::DrawCircle(int32_t centreX, int32_t centreY, int32_t radius)
{
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	SDL_SetRenderDrawColor(targetRenderer, 0, 0, 0, 255);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(targetRenderer, centreX + x, centreY - y);
		SDL_RenderDrawPoint(targetRenderer, centreX + x, centreY + y);
		SDL_RenderDrawPoint(targetRenderer, centreX - x, centreY - y);
		SDL_RenderDrawPoint(targetRenderer, centreX - x, centreY + y);
		SDL_RenderDrawPoint(targetRenderer, centreX + y, centreY - x);
		SDL_RenderDrawPoint(targetRenderer, centreX + y, centreY + x);
		SDL_RenderDrawPoint(targetRenderer, centreX - y, centreY - x);
		SDL_RenderDrawPoint(targetRenderer, centreX - y, centreY + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}

void Vanity::UpdateMousePosision() {
	SDL_GetMouseState(&mX, &mY);
}

void Vanity::CaptureInputText() {
	if (!activeTextbox) return;


	int nK;
	char key = NULL;
	Uint32 now = SDL_GetTicks();
	const Uint8* keys = SDL_GetKeyboardState(&nK);
	std::string value = activeTextbox->GetValue();
	int charLimit = activeTextbox->GetCharLimit();

	// Enable capital letters if capslock is pressed
	if (keys[SDL_SCANCODE_CAPSLOCK] && now - delta >= 300) {
		capsLockEnabled = !capsLockEnabled;
		delta = now;
	}

	// Delete last character from input string if backspace is pressed
	if (keys[SDL_SCANCODE_BACKSPACE] && now - delta >= 110) {
		activeTextbox->SetValue(value.substr(0, value.size() - 1));
		delta = now;

		// Reset textbox cursor on key input
		textboxCursorDelta = now;
		drawTextBoxCursor = true;
		return;
	}

	// If character limit has been reached, jump out 
	if (charLimit && value.length() >= charLimit) return;

	for (int i = 0; i < nK; i++) {
		if (keys[i] && ValidKey(i)) {
			if (keys[SDL_SCANCODE_LSHIFT] || capsLockEnabled) {
				key = toupper(SDL_GetKeyFromScancode(SDL_Scancode(i)));
			}
			else {
				key = SDL_GetKeyFromScancode(SDL_Scancode(i));
			}
			// Reset textbox cursor on key input
			textboxCursorDelta = now;
			drawTextBoxCursor = true;
		}
	}

	// If pressed key is not the same as last key, then just print immediately 
	if (key != lastPressedKey && key) {
		activeTextbox->SetValue(value += key);
		lastPressedKey = key;
		delta = now;
	}
	// If same key, check if enough time has passed since last key press. 
	// If enough time has passed, print pressed key
	else if (now - delta >= 200 && key) {
		activeTextbox->SetValue(value += key);
		lastPressedKey = key;
		delta = now;
	}
}

bool Vanity::ValidKey(int key) {
	// Determines what keys are considered valid (valid as in printable)
	if (key >= 4 && key <= 39 || key >= 44 && key <= 49 || key >= 54 && key <= 56) {
		return true;
	}
	return false;
}

bool Vanity::OnMouseHover(int x, int y, int width, int height) {
	// If mouse hovers over button
	if (mX >= x && mX <= x + width && mY >= y && mY <= y + height) {
		return true;
	}
	return false;
}

void Vanity::UpdateMouseButtonState() {
	Uint32 mb = SDL_GetMouseState(nullptr, nullptr);
	if (mb & SDL_BUTTON(1)) {
		if (!leftMouseHeld) {
			leftMouseHeld = true;
			leftMousePressed = true;
		}
		else {
			leftMousePressed = false;
		}
	}
	else {
		leftMouseHeld = false;
		leftMousePressed = false;
	}
}

TTF_Font* Vanity::OpenFont(std::string fontUrl, int size) {
	TTF_Font* font = TTF_OpenFont(fontUrl.c_str(), size);
	if (!font) {
		char t[] = "Font error";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, t, TTF_GetError(), nullptr);
		exit(0);
	}
	return font;
}

// Handles inherited positioning and display state of element 
bool Vanity::InheritStateFromParent(Division* parent, int& elementX, int& elementY, bool elementDisplayState) {
	while (parent) {
		elementX += parent->GetX() + parent->GetPaddingLeft();
		elementY += parent->GetY() + parent->GetPaddingTop();
		if (!parent->GetDisplayState()) elementDisplayState = false;
		parent = parent->GetParent();
	}
	return elementDisplayState;
}

void Vanity::RenderBackgroundImage(SDL_Texture* image, int width, int height, int x, int y) {
	// Create image rectangle data
	SDL_Rect rect;
	rect.w = width;
	rect.h = height;
	rect.x = x;
	rect.y = y;

	SDL_RenderCopy(targetRenderer, image, nullptr, &rect);
}


// TODO: Finish or remove the two following methods
void Vanity::prepareNewSnapshotFrame() {
	// Destroy old snapshot frame
	/*SDL_DestroyTexture(snapshotFrame);*/

	// Create texture
	snapshotFrame = SDL_CreateTexture(targetRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, viewportWidth, viewportHeight);
	SDL_SetTextureBlendMode(snapshotFrame, SDL_BLENDMODE_BLEND);

	// Give drawing focus to texture
	SDL_SetRenderTarget(targetRenderer, snapshotFrame);

	// Clear texture
	SDL_SetRenderDrawColor(targetRenderer, 255, 255, 255, 0);
	SDL_RenderClear(targetRenderer);
}

void Vanity::finalizeNewSnapshotFrame() {
	/*SDL_SetRenderDrawColor(Mildred::GetRenderer(), 0, 255, 0, 255);
	SDL_RenderDrawLine(Mildred::GetRenderer(), 20, 20, 100, 100);*/
	SDL_Rect rect;
	rect.w = viewportWidth;
	rect.h = viewportHeight;
	rect.x = 0;
	rect.y = 0;

	// Return drawing focus to screen 
	SDL_SetRenderTarget(targetRenderer, nullptr);
	//Mildred::SetRenderDrawColor(255, 255, 255, 255);
	// Draw texture to screen
	SDL_RenderCopy(targetRenderer, snapshotFrame, nullptr, &rect);

}

void Vanity::Rerender() {
	rerender = true;
}

SDL_Texture* Vanity::LoadImage(std::string imagePath) {
	SDL_Texture* image = IMG_LoadTexture(targetRenderer, imagePath.c_str());
	if (!image) {
		char t[] = "Image error";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, t, IMG_GetError(), nullptr);
		exit(0);
	}
	return image;
}

bool Vanity::IsRunning() {
	return isRunning;
}

void Vanity::Prepare() {
	SDL_SetRenderDrawColor(targetRenderer, 255, 255, 255, 255);
	SDL_RenderClear(targetRenderer);

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			std::cout << "Quitting..." << std::endl;
			isRunning = false;
			SDL_Quit();
			break;
		}
	}
}

void Vanity::SetActiveTextbox(Textbox* textbox) {
	activeTextbox = textbox;
}

SDL_Renderer* Vanity::CreateRenderingContext(std::string title) {
	SDL_Window* window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		viewportWidth, viewportHeight, SDL_WINDOW_SHOWN);

	// Check what graphics backend is used 
	SDL_RendererInfo info = {};

	// Graphics device index to create renderer using correct graphics API, -1 is default
	int deviceIndex = -1;

	for (int i = 0; i < SDL_GetNumRenderDrivers(); i++) {
		SDL_GetRenderDriverInfo(i, &info);
		if (info.name == std::string("direct3d11")) {
			deviceIndex = i;
			break;
		}
	}

	std::cout << "Using graphics API: " << info.name << std::endl;

	// Captures mouse to window
	//SDL_SetRelativeMouseMode(SDL_TRUE);

	// NOTE: Remove SDL_RENDERER_PRESENTVSYNC flag to turn off v-sync
	SDL_Renderer* renderer = nullptr;

	if (vsync)
		renderer = SDL_CreateRenderer(window, deviceIndex, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	else
		renderer = SDL_CreateRenderer(window, deviceIndex, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	// To enable aplha channel on draw calls
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "BEST");
	return targetRenderer = renderer;
}

void Vanity::EnableVsync() {
	vsync = true;
}

void Vanity::DisableVsync() {
	vsync = false;
}

int Vanity::GetViewportWidth() {
	return viewportWidth;
}

int Vanity::GetViewportHeight() {
	return viewportHeight;
}