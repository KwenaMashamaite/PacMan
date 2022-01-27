////////////////////////////////////////////////////////////////////////////////
// Pac-Man clone
//
// Copyright (c) 2021 Kwena Mashamaite (kwena.mashamaite1@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include "Grid.h"
#include "Animations/GridAnimation.h"
#include <IME/core/grid/Grid2D.h>
#include <IME/core/scene/Scene.h>
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    Grid::Grid(ime::Grid2D &grid) :
        grid_{grid}
    {
        // Set render layers for different game object. Note that by default, IME sorts
        // render layers by the order in which they are created
        ime::RenderLayerContainer& renderLayers = grid.getScene().getRenderLayers();
        renderLayers.removeByName("default"); // This layer is replaced by the background layer
        renderLayers.create("background");
        renderLayers.create("Walls");
        renderLayers.create("Sensors");
        renderLayers.create("Fruits");
        renderLayers.create("Pellets");
        renderLayers.create("PacMans");
        renderLayers.create("Ghosts");

        // Instead of creating the visual grid ourself we will use a pre-made one
        // from an image file and render our game objects on top of it.
        ime::Animation::Ptr animation = GridAnimation().get();

        animation->onComplete([this](ime::Animation*) {
            if (onAnimFinish_)
                onAnimFinish_();
        });

        background_ = animation->getSpriteSheet().getSprite(ime::Index{0, 0});
        renderLayers.add(background_, 0, "background");
        background_.getAnimator().addAnimation(std::move(animation));

        background_.setOrigin(background_.getLocalBounds().width / 2.0f, background_.getLocalBounds().height / 2.0f);
        background_.scale(2.14f, 2.1f);
    }

    ///////////////////////////////////////////////////////////////
    void Grid::loadFromFile(const std::string& filename) {
        grid_.loadFromFile(filename);
    }

    ///////////////////////////////////////////////////////////////
    void Grid::setVisible(bool visible) {
        grid_.getRenderer().setVisible(visible);
    }

    ///////////////////////////////////////////////////////////////
    void Grid::setPosition(ime::Vector2f pos) {
        grid_.setPosition(pos.x, pos.y);
    }

    ///////////////////////////////////////////////////////////////
    void Grid::setBackgroundImagePosition(ime::Vector2f pos) {
        background_.setPosition(pos);
    }

    ///////////////////////////////////////////////////////////////
    void Grid::addActor(ime::GridObject::Ptr object, ime::Index index) {
        assert(object && "Object must not be a nullptr");
        std::string renderLayer = object->getClassName() + "s";

        grid_.addChild(object.get(), index);
        std::string group = object->getClassName();
        grid_.getScene().getGameObjects().add(group, std::move(object), 0, renderLayer);
    }

    ///////////////////////////////////////////////////////////////
    ime::GridObject* Grid::getActorById(int id) const {
        return grid_.getChildWithId(id);
    }

    ///////////////////////////////////////////////////////////////
    void Grid::removeActor(int id) {
        grid_.removeChildWithId(id);
    }

    ///////////////////////////////////////////////////////////////
    void Grid::forEachCell(const ime::Callback<const ime::Tile&>& callback) {
        grid_.forEachTile([&callback](const ime::Tile& tile) {
            callback(tile);
        });
    }

    ///////////////////////////////////////////////////////////////
    void Grid::forEachActor(const ime::Callback<ime::GridObject*> &callback) {
        grid_.forEachChild([&callback](ime::GridObject* actor) {
            callback(actor);
        });
    }

    ///////////////////////////////////////////////////////////////
    ime::Time Grid::playFlashAnimation() {
        background_.getAnimator().startAnimation("flash");
        return background_.getAnimator().getActiveAnimation()->getDuration();
    }

    ///////////////////////////////////////////////////////////////
    void Grid::onAnimationFinish(ime::Callback<> callback) {
        onAnimFinish_ = std::move(callback);
    }

    ///////////////////////////////////////////////////////////////
    bool Grid::isAnimationPlaying() {
        return background_.getAnimator().isAnimationPlaying();
    }

    ///////////////////////////////////////////////////////////////
    void Grid::update(ime::Time deltaTime) {
        background_.updateAnimation(deltaTime);
    }

    ///////////////////////////////////////////////////////////////
    ime::Scene& Grid::getScene() {
        return grid_.getScene();
    }

} // namespace pm
