#include "ResourceBrowser.hpp"

namespace URDE
{
#define BROWSER_MARGIN 8

bool ResourceBrowser::navigateToPath(const HECL::ProjectPath& pathIn)
{
    if (pathIn.getPathType() == HECL::ProjectPath::Type::File)
        m_path = pathIn.getParentPath();
    else
        m_path = pathIn;

    m_comps = m_path.getPathComponents();

    HECL::DirectoryEnumerator dEnum(m_path.getAbsolutePath(), HECL::DirectoryEnumerator::Mode::DirsThenFilesSorted,
                                    m_state.sortColumn==State::SortColumn::Size,
                                    m_state.sortDir==Specter::SortDirection::Descending,
                                    true);
    m_fileListingBind.updateListing(dEnum);
    m_view->m_fileListing.m_view->selectRow(-1);
    m_view->m_fileListing.m_view->updateData();
    m_view->m_pathButtons.m_view->setButtons(m_comps);

    m_view->updateSize();

    return true;
}

void ResourceBrowser::pathButtonActivated(size_t idx)
{
    if (idx >= m_comps.size())
        return;

    HECL::SystemString dir;
    bool needSlash = false;
    size_t i = 0;
    for (const HECL::SystemString& d : m_comps)
    {
        if (needSlash)
            dir += _S('/');
        if (d.compare(_S("/")))
            needSlash = true;
        dir += d;
        if (++i > idx)
            break;
    }
    navigateToPath(HECL::ProjectPath(*m_vm.project(), dir));
}

void ResourceBrowser::View::resized(const boo::SWindowRect& root, const boo::SWindowRect& sub)
{
    float pf = rootView().viewRes().pixelFactor();

    boo::SWindowRect pathRect = sub;
    pathRect.location[0] += BROWSER_MARGIN * pf;
    pathRect.location[1] += pathRect.size[1] - (BROWSER_MARGIN + 20) * pf;
    m_pathButtons.m_view->resized(root, pathRect);
}
void ResourceBrowser::View::draw(boo::IGraphicsCommandQueue* gfxQ)
{
    m_pathButtons.m_view->draw(gfxQ);
    m_fileListing.m_view->draw(gfxQ);
}

}
