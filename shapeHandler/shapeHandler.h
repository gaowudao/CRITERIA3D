#ifndef SHAPEHANDLER_H
#define SHAPEHANDLER_H

    #include <string>
    #include <shapefil.h>
    #include "shape.h"

    class Crit3DShapeHandler
    {
    protected:
        SHPHandle	m_handle;
        int			m_count;
        int			m_type;

    public:
        Crit3DShapeHandler();
        ~Crit3DShapeHandler();

        bool open(std::string filename);
        void close();
        bool getShape(int index, Crit3DShape &shape);
        int	getShapeCount();
        int	getType();
        std::string	getTypeString();
    };


#endif // SHAPEHANDLER_H
