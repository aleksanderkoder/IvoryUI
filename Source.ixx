import VanityUI;
#include <iostream>

int main(int argc, char* argv[])
{
    Vanity::Setup(1920, 1080);
    Vanity::DisableVsync(); 

    // Create a rendering context via Ivory. Can also pass an existing renderer to the Ivory Setup() method instead. 
    SDL_Renderer* renderer = Vanity::CreateRenderingContext("Vanity UI"); 

    SDL_Color black = { 0,0,0,255 };
    Label* l = Vanity::CreateLabel("Div 3");
    Button* b = Vanity::CreateButton("Page 2");

    auto tb = Vanity::CreateTextbox("Text char limit", 350);
    //auto img = Vanity::CreateImage("db3991d70eff2556d448c9d911e71a11_400x400.jpeg", 600, 0, 50, 50);
    auto cb = Vanity::CreateCheckbox(550); 
    auto sl = Vanity::CreateSlider(650, 500); 

    tb->SetBackgroundImage("db3991d70eff2556d448c9d911e71a11_400x400.jpeg"); 

    auto div = Vanity::CreateDivision(0, 0, 600, 600);
    auto div2 = Vanity::CreateDivision(100, 100, 200, 200);
    auto div3 = Vanity::CreateDivision(100, 100, 200, 200);
    auto div4 = Vanity::CreateDivision(500, 0, 200, 200);
    div->AddChild(div2); 
    div2->AddChild(div3); 
    div4->SetColor({ 250,0,0,255 }); 

    auto btnGiveFocus = Vanity::CreateButton("Give focus to textbox");
    btnGiveFocus->SetWidth(200); 

    div3->AddChild(btnGiveFocus)->AddChild(b)->AddChild(div4)->AddChild(tb)->AddChild(l)->AddChild(cb);

    auto autoLayoutBtn1 = Vanity::CreateButton("absolute layout", 100, 50);
    auto autoLayoutBtn3 = Vanity::CreateButton("auto layout");
    auto autoLayoutBtn4 = Vanity::CreateButton("auto layout 2");
    auto autoLayoutBtn2 = Vanity::CreateTextbox("Textbox", 50, 50);

    autoLayoutBtn3->SetWidth("50%");

    auto div5ForAutoLayout = Vanity::CreateDivision(100, 100);
    auto div6 = Vanity::CreateDivision(150, 150, 300, 300);

    auto btn1 = Vanity::CreateButton("test", 0, 0, 290);
    auto txt1 = Vanity::CreateTextbox("Textbox", 0, 50); 
    auto sli1 = Vanity::CreateSlider(0, 100); 
    auto sli2 = Vanity::CreateSlider(0, 100);
    auto div1 = Vanity::CreateDivision(0, 150, 30, 30); 
    auto chk1 = Vanity::CreateCheckbox(0, 200); 
    auto chk2 = Vanity::CreateCheckbox(0, 50);
    auto img = Vanity::CreateImage("db3991d70eff2556d448c9d911e71a11_400x400.jpeg", 0, 300); 
    auto lbl = Vanity::CreateLabel("Label", 0, 420);

    div6->AddChild(btn1)->AddChild(div1)->AddChild(sli1)->AddChild(txt1)->AddChild(chk1)->AddChild(img)->AddChild(lbl);

    div5ForAutoLayout->AddChild(autoLayoutBtn1)->AddChild(autoLayoutBtn2)
        ->AddChild(autoLayoutBtn4)->AddChild(div6)->AddChild(sli2);

    div5ForAutoLayout->SetColor({ 0, 255, 0, 255 });

    div6->SetColor({ 255, 0, 0, 255 });

    //div6->SetPadding({ 0, 0, 0, 0 }); 
    //div5ForAutoLayout->SetPadding({ 0, 0, 0, 0 });

    //btn1->SetDimensions("20%", "50%"); 

    auto btn = Vanity::CreateButton("Back to page 1");

    //b->SetDimensions("50%", "100%");

    b->SetAnimation(-100, 300, "linear", 3000); 
    b->Animate(); 

    Page* p = Vanity::CreatePage();
    Page* p2 = Vanity::CreatePage();

    //p->AddElement(div)->AddElement(sl);

    p->AddElement(div5ForAutoLayout);
    auto knappForTest = Vanity::CreateButton("Knapp for testing");

    p->AddElement(div6); 
    div6->AddChild(knappForTest)->AddChild(b);

    p->AddElement(autoLayoutBtn3); 
    p->AddElement(chk2); 

    p2->AddElement(btn); 

    Vanity::DisplayPage(p);

    //div6->HorizontallyAlignElementsCenter(); 
    //div6->VerticallyAlignElementsCenter(); 
    //div6->AlignElementsTop(); 
    //div6->AlignElementsRight();
    //div6->AlignElementsLeft(); 
    //div6->AlignElementsBottom();

    //btn1->AlignRight(); 
    //btn1->AlignLeft();
 

    while (Vanity::IsRunning()) {

        Vanity::Prepare(); 

        //tb->SetValue(std::to_string(sl->GetValue())); 

        if (b->Clicked()) {
            std::cout << "Button pressed!" << std::endl;
            //Vanity::DisplayPage(p2); 
            b->SetLabel("Changed a lot");
        }

        if (btn1->Clicked()) {
            //Vanity::DisplayPage(p);
            std::cout << "Button clicked" << std::endl; 
            //btn1->GetParent()->AlignElementsRight();
            //btn1->HorizontallyAlignCenter(); 
        }

        if (btn->Clicked()) {
            Vanity::DisplayPage(p);
        }

        if (btnGiveFocus->Clicked()) {
            tb->Focus(); 
        }

        if (txt1->Clicked())
            std::cout << "Textbox clicked" << std::endl;

        if (img->Clicked())
            std::cout << "Image clicked" << std::endl;

        if (sli1->Clicked())
            std::cout << "Slider clicked" << std::endl;

        if (div6->Clicked()) {
            std::cout << "Division clicked" << std::endl;
        }

        if (chk1->Clicked())
            std::cout << "Checkbox clicked" << std::endl;

        if (lbl->Clicked())
            std::cout << "Label clicked" << std::endl;

        Vanity::Render();
        //SDL_RenderPresent(renderer);

    }

    //SDL_Delay(10000);
    return 0;
}