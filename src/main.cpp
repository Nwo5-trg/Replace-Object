#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/FLAlertLayer.hpp>

using namespace geode::prelude;

class $modify(EditUI, EditorUI) {
    struct Object {
        GameObject* obj;
        float scaleX, scaleY, rotation;
        bool flipX, flipY;
        std::vector<short> groups;
    };

    void replaceObjects(CCObject* sender) {
        if (auto selectedObjs = getSelectedObjects(); selectedObjs->count() > 1) {
            auto replaceObj = static_cast<GameObject*>(selectedObjs->objectAtIndex(0));
            auto replaceWithObj = static_cast<GameObject*>(selectedObjs->objectAtIndex(1));
            auto replaceObjID = replaceObj->m_objectID;
            auto replaceWithObjID = replaceWithObj->m_objectID;
            CCArray tempObjs;
            tempObjs.addObject(replaceObj);
            tempObjs.addObject(replaceWithObj);
            
            auto copyValues = Mod::get()->getSettingValue<bool>("copy-values");

            deselectAll();
            selectObjects(&tempObjs, true);
            m_trashBtn->activate();
            
            auto objs = selectedObjs->count() > 2 ? selectedObjs : m_editorLayer->m_objects;
            
            std::vector<Object> replaceVector;
            CCArray replaceArray;
            for (int i = 0; i < objs->count(); i++) {
                // if (i < 2) if (selectedObjs->count() < 3) return;
                auto obj = static_cast<GameObject*>(objs->objectAtIndex(i));
                if (obj->m_objectID == replaceObjID) {
                    std::vector<short> groups;
                    if (obj->m_groups) for (short group : *obj->m_groups) if (group != 0) groups.emplace_back(group);
                    replaceVector.emplace_back(Object{obj, obj->m_scaleX, obj->m_scaleY, obj->getObjectRotation(), obj->m_isFlipX, obj->m_isFlipY, groups});
                    replaceArray.addObject(obj);
                }
            }

            deselectAll();
            selectObjects(&replaceArray, true);
            m_trashBtn->activate();
            
            CCArray replacedObjects;
            for (auto& currentObj : replaceVector) {
                auto obj = m_editorLayer->createObject(replaceWithObjID, currentObj.obj->getPosition(), false);
                if (copyValues) {
                    obj->setScaleX(currentObj.scaleX);
                    obj->setScaleY(currentObj.scaleY);
                    obj->setRotationX(currentObj.rotation);
                    obj->setFlipX(currentObj.flipX);
                    obj->setFlipY(currentObj.flipY);
                    for (short group : currentObj.groups) obj->addToGroup(group);
                }
                replacedObjects.addObject(obj);
            }

            deselectAll();
            if (Mod::get()->getSettingValue<bool>("select-on-replace")) this->selectObjects(&replacedObjects, true);
            updateButtons();
        }
        else FLAlertLayer::create("Replace", "You have to select 2 or more objects for this function to work, so, uh, do that :3", "Ok")->show();
    }  
    
    void createMoveMenu() {
		EditorUI::createMoveMenu();
        auto* btn = this->getSpriteButton("Sheet.png"_spr, menu_selector(EditUI::replaceObjects), nullptr, 1);
        m_editButtonBar->m_buttonArray->addObject(btn);
        auto rows = GameManager::sharedState()->getIntGameVariable("0049");
        auto cols = GameManager::sharedState()->getIntGameVariable("0050");
        m_editButtonBar->reloadItems(rows, cols);
	}
};