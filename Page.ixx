export module Page;

import Elements;
import <vector>;

export class Page
{
	private:
		std::vector<Element*>* elements; 

	public:
		Page();

		Page* AddElement(Element* element);
		Page* AddElement(Division* division); 

		Page* RemoveElement(Element* element);
		Page* RemoveElement(Division* division);

		std::vector<Element*>* GetElements(); 
		std::vector<Button*>* GetButtons();
		std::vector<Label*>* GetLabels();
		std::vector<Checkbox*>* GetCheckboxes();
		std::vector<Textbox*>* GetTextboxes(); 
		std::vector<Image*>* GetImages();
		std::vector<Slider*>* GetSliders(); 
		std::vector<Division*>* GetDivisions();
};

Page::Page() {
	this->elements = new std::vector<Element*>();
}

Page* Page::AddElement(Element* element) {
	this->elements->push_back(element);
	return this;
}

Page* Page::AddElement(Division* division) {
	this->elements->push_back(division);

	// Add all elements to page contained within the divison
	/*auto elements = division->GetElements();
	for (int i = 0; i < elements->size(); i++) {
		if (Division* div = dynamic_cast<Division*>((*elements)[i]))
			this->AddElement(div);
		else
			this->AddElement((*elements)[i]);
	}*/
	return this;
}

Page* Page::RemoveElement(Element* element) {
	auto elements = this->elements;
	for (int i = 0; i < elements->size(); i++) {
		auto curr = (*elements)[i];
		if (curr == element)
			elements->erase(elements->begin() + i);
	}
	return this;
}


Page* Page::RemoveElement(Division* division) {
	auto divs = this->elements;
	for (int i = 0; i < divs->size(); i++) {
		auto curr = (*divs)[i];
		if (curr == division)
			divs->erase(divs->begin() + i);
	}
	return this;
}

std::vector<Element*>* Page::GetElements() {
	auto elements = new std::vector<Element*>();
	for (int i = 0; i < this->elements->size(); i++) {
		if (Division* div = dynamic_cast<Division*>((*this->elements)[i])) {
			elements->push_back(div);
			auto divElements = div->GetElements();
			elements->insert(elements->end(), divElements->begin(), divElements->end());
			continue;
		}
		elements->push_back((*this->elements)[i]);
	}
	return elements;
}

std::vector<Button*>* Page::GetButtons() {
	auto buttons = new std::vector<Button*>();
	for (int i = 0; i < elements->size(); i++) {
		if (Button* button = dynamic_cast<Button*>((*elements)[i])) {
			buttons->push_back(button);
		}
	}
	return buttons;
}

std::vector<Label*>* Page::GetLabels() {
	auto labels = new std::vector<Label*>();
	for (int i = 0; i < elements->size(); i++) {
		if (Label* label = dynamic_cast<Label*>((*elements)[i])) {
			labels->push_back(label);
		}
	}
	return labels;
}

std::vector<Checkbox*>* Page::GetCheckboxes() {
	auto checkboxes = new std::vector<Checkbox*>();
	for (int i = 0; i < elements->size(); i++) {
		if (Checkbox* checkbox = dynamic_cast<Checkbox*>((*elements)[i])) {
			checkboxes->push_back(checkbox);
		}
	}
	return checkboxes;
}

std::vector<Textbox*>* Page::GetTextboxes() {
	auto textboxes = new std::vector<Textbox*>();
	for (int i = 0; i < elements->size(); i++) {
		if (Textbox* textbox = dynamic_cast<Textbox*>((*elements)[i])) {
			textboxes->push_back(textbox);
		}
	}
	return textboxes;
}

std::vector<Image*>* Page::GetImages() {
	auto images = new std::vector<Image*>();
	for (int i = 0; i < elements->size(); i++) {
		if (Image* image = dynamic_cast<Image*>((*elements)[i])) {
			images->push_back(image);
		}
	}
	return images;
}

std::vector<Slider*>* Page::GetSliders() {
	auto sliders = new std::vector<Slider*>();
	for (int i = 0; i < elements->size(); i++) {
		if (Slider* slider = dynamic_cast<Slider*>((*elements)[i])) {
			sliders->push_back(slider);
		}
	}
	return sliders;
}

std::vector<Division*>* Page::GetDivisions() {
	auto divisions = new std::vector<Division*>();
	for (int i = 0; i < elements->size(); i++) {
		if (Division* division = dynamic_cast<Division*>((*elements)[i])) {
			divisions->push_back(division);
		}
	}
	return divisions;
}
