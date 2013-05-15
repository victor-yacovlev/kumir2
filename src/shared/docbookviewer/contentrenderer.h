#ifndef CONTENTRENDERER_H
#define CONTENTRENDERER_H

namespace DocBookViewer {

class ContentRenderer
{
public:
    virtual void reset() = 0;
    virtual void addData(const class DocBookModel * data) = 0;
};

}

#endif
