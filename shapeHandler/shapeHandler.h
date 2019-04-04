#ifndef SHAPEHANDLER_H
#define SHAPEHANDLER_H

    #include <string>
    #include <shapefil.h>
    #include "shapeObject.h"

    class Crit3DShapeHandler
    {
    protected:
        SHPHandle	m_handle;
        DBFHandle   m_dbf;
        int			m_count;
        int			m_type;
        int         m_fields;

    public:
        Crit3DShapeHandler();
        ~Crit3DShapeHandler();

        bool open(std::string filename);
        void close();
        bool getShape(int index, ShapeObject &shape);
        int	getShapeCount();
        int	getType();
        int	getFieldNumbers();
        std::string	getTypeString();
    };


#endif // SHAPEHANDLER_H
