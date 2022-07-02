#pragma once

#include <memory>
#include "event/Event.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::data
{
    class Text;
    class Buildings;
}

namespace mdcii::map
{
    class Map;
}

//-------------------------------------------------
// EditorGui
//-------------------------------------------------

namespace mdcii
{
    /**
     * Different menus for the editor.
     */
    class EditorGui
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        /**
         * Constructs a new EditorGui object.
         *
         * @param t_text Labels for all menus and buildings in different languages.
         * @param t_map The Map object to edit.
         * @param t_buildings Access to all building objects.
         */
        EditorGui(
            std::shared_ptr<data::Text> t_text,
            std::shared_ptr<map::Map> t_map,
            std::shared_ptr<data::Buildings> t_buildings
        );

        EditorGui(const EditorGui& t_other) = delete;
        EditorGui(EditorGui&& t_other) noexcept = delete;
        EditorGui& operator=(const EditorGui& t_other) = delete;
        EditorGui& operator=(EditorGui&& t_other) noexcept = delete;

        ~EditorGui() noexcept;

        //-------------------------------------------------
        // Gui
        //-------------------------------------------------

        /**
         * Buttons for rotating the map left and right.
         */
        void RotateMapGui() const;

        /**
         * Shows a list of all workshops.
         * When a building is selected, an event (BauGfxSelectedEvent) is created.
         */
        void AllWorkshopsGui() const;

        /**
         * Shows a single selected workshop.
         */
        void WorkshopGui(event::SelectedBauGfx& t_selectedBauGfx) const;

        /**
         * Shows some debug values.
         */
        void DebugGui() const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The configured language from the config.ini file.
         */
        std::string m_lang;

        /**
         * Labels for all menus and buildings in different languages.
         */
        std::shared_ptr<data::Text> m_text;

        /**
         * The Map object to edit.
         */
        std::shared_ptr<map::Map> m_map;

        /**
         * Access to all building objects.
         */
        std::shared_ptr<data::Buildings> m_buildings;
    };
}
