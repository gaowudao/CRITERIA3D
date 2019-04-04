#ifndef SHAPEHANDLER_H
#define SHAPEHANDLER_H

    #include <string>
    #include <vector>
    #include <shapefil.h>
    #include "shapeObject.h"

    class Crit3DShapeHandler
    {
    protected:
        SHPHandle	m_handle;
        DBFHandle   m_dbf;
        int			m_count;
        int			m_type;
        unsigned int         m_fields;
        std::vector<std::string> m_fieldsList;

    public:
        Crit3DShapeHandler();
        ~Crit3DShapeHandler();

        bool open(std::string filename);
        void close();
        bool getShape(int index, ShapeObject &shape);
        int	getShapeCount();
        int	getType();
        unsigned int getFieldNumbers();
        std::string	getFieldName(unsigned long fieldPos);
        std::string	getTypeString();
    };


#endif // SHAPEHANDLER_H
