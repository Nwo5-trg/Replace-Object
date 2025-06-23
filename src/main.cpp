#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(EditUI, EditorUI) {
    void onReplaceObjects(CCObject* sender) {
        auto mod = Mod::get();

        auto editor = m_editorLayer;

        auto selectedObjs = this->getSelectedObjects();
        int count = selectedObjs->count();

        if (count < 2) {
            FLAlertLayer::create(
                "Replace", 
                "You have to select 2 or more objects for this function to work, so, uh, do that :3", 
                "Ok"
            )->show();
            return;
        }

        auto replaceObj = static_cast<GameObject*>(selectedObjs->objectAtIndex(0));
        auto withObj = static_cast<GameObject*>(selectedObjs->objectAtIndex(1));
        int replaceID = replaceObj->m_objectID;
        int withID = withObj->m_objectID;

        auto objectsToReplace = selectedObjs;
        if (count == 2) {
            auto objs = editor->m_objects;
            auto allObjectsToReplace = CCArray::create();
            for (auto obj : CCArrayExt<GameObject*>(objs)) if (obj->m_objectID == replaceID) allObjectsToReplace->addObject(obj);
            objectsToReplace = allObjectsToReplace;
        }

        auto objString = this->copyObjects(objectsToReplace, true, true);
        objString = utils::string::replace( // jesus take the wheel
            objString, 
            "1," + std::to_string(replaceID) + ",", 
            "1," + std::to_string(withID) + ","
        );

        if (!mod->getSettingValue<bool>("delete-on-replace")) objectsToReplace->removeObject(withObj);

        for (auto obj : CCArrayExt<GameObject*>(objectsToReplace)) editor->removeObject(obj, false);

        auto pastedObjs = pasteObjects(objString, true, true);
        if (!mod->getSettingValue<bool>("delete-on-replace")) pastedObjs->addObject(withObj);
        
        this->deselectAll();
        if (mod->getSettingValue<bool>("select-on-replace")) {
            this->selectObjects(pastedObjs, true);
        }

        this->updateButtons();
        this->updateObjectInfoLabel();
        this->updateGridNodeSize();
    }  
    
    void createMoveMenu() {
		EditorUI::createMoveMenu();
        auto* btn = this->getSpriteButton("Sheet.png"_spr, menu_selector(EditUI::onReplaceObjects), nullptr, 1);
        m_editButtonBar->m_buttonArray->addObject(btn);
        auto rows = GameManager::sharedState()->getIntGameVariable("0049");
        auto cols = GameManager::sharedState()->getIntGameVariable("0050");
        m_editButtonBar->reloadItems(rows, cols);
	}
};