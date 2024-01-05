#include "MenuLeaderboard.h"

#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "Engine/UI/Elements/RectImage.h"

void MenuLeaderboard::Init()
{
     UI::Builder builder = UI::Builder()
          .Push(UI::Container(UI::Transform::FromRect(600.0f, -300.0f, 0.5f)))
               .Add(UI::RectImage(ResTexture("UI/T_Rectangle.png"), UI::Margin(5.0f)))
               .Add(UI::Label("Leaderboard: ", 0.0f, UI::Transform::Fill(10.0f))) 
               .Push(UI::Container(UI::Transform::Fill({ 10.0f, { 50.0f, 10.0f }})))
                    .Add(UI::RectImage(ResTexture("UI/T_Rectangle.png"), UI::Margin(5.0f)))     
                    .Push(UI::List(UI::Transform::Fill(10.0f), 0.0f, 50.0f));
                         
     
     struct Entry
     {
          String Name;
          int Score; 
     };
     Vector<Entry> entries;
     entries.push_back({"Harald", 999});
     entries.push_back({"Bastian", 727});
     entries.push_back({"Tobias", 123});

     auto addEntry = [&](const Entry& InEntry)
     {
          builder.Push(UI::List(UI::Transform::Fill(), 0.0f, 0.0f, UI::List::FlowDirection::HORIZONTAL))
               .Add(UI::Label(InEntry.Name))
               .Add(UI::Label(std::to_string(InEntry.Score), Vec2F(1.0f, 0.0f)))
               .Pop();
     };

     for (auto& entry : entries)
          addEntry(entry); 

     UI = builder.Build(); 
}

void MenuLeaderboard::Update(double InDelta)
{
    
}
