/*
 Copyright (C) 2010-2014 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TrenchBroom__EntityLinkRenderer__
#define __TrenchBroom__EntityLinkRenderer__

#include "Color.h"
#include "Model/ModelTypes.h"
#include "Renderer/Renderable.h"
#include "Renderer/Vertex.h"
#include "Renderer/VertexArray.h"
#include "View/ViewTypes.h"

namespace TrenchBroom {
    namespace Model {
        class EditorContext;
    }
    
    namespace Renderer {
        class RenderBatch;
        class RenderContext;
        
        class EntityLinkRenderer : public Renderable {
        private:
            typedef VertexSpecs::P3C4::Vertex Vertex;
            
            View::MapDocumentWPtr m_document;
            
            Color m_defaultColor;
            Color m_selectedColor;
            
            VertexArray m_entityLinks;
            bool m_valid;
        public:
            EntityLinkRenderer(View::MapDocumentWPtr document);
            
            void setDefaultColor(const Color& color);
            void setSelectedColor(const Color& color);
            
            void render(RenderContext& renderContext, RenderBatch& renderBatch);
            void invalidate();
        private:
            void doPrepare(Vbo& vbo);
            void doRender(RenderContext& renderContext);
        private:
            void validate();
            
            class MatchEntities;
            class CollectEntitiesVisitor;
            
            class CollectLinksVisitor;
            class CollectAllLinksVisitor;
            class CollectTransitiveSelectedLinksVisitor;
            class CollectDirectSelectedLinksVisitor;

            Vertex::List links() const;
            Vertex::List allLinks() const;
            Vertex::List transitiveSelectedLinks() const;
            Vertex::List directSelectedLinks() const;
            Vertex::List collectSelectedLinks(CollectLinksVisitor& collectLinks) const;
            
            EntityLinkRenderer(const EntityLinkRenderer& other);
            EntityLinkRenderer& operator=(const EntityLinkRenderer& other);
        };
    }
}

#endif /* defined(__TrenchBroom__EntityLinkRenderer__) */
