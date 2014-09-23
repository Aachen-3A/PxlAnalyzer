//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "Pxl/Pxl/interface/pxl/core/FileFactory.hh"
#include "Pxl/Pxl/interface/pxl/core/logging.hh"

#undef PXL_LOG_MODULE_NAME
#define PXL_LOG_MODULE_NAME "pxl::FileFactory"

namespace pxl
{

FileFactory::FileFactory()
{
}

FileFactory& FileFactory::instance()
{
	static FileFactory f;
	return f;
}

FileImpl *FileFactory::create(const std::string& id)
{
	std::map<std::string, const FileProducerInterface *>::iterator result;
	result = _Producers.find(id);
	if (result == _Producers.end())
		return 0;
	else
		return (*result).second->create();
}

void FileFactory::registerProducer(const std::string& id,
		const FileProducerInterface* producer)
{
	PXL_LOG_INFO << "register object file for " << id;
	_Producers[id] = producer;
}

void FileFactory::unregisterProducer(const FileProducerInterface* producer)
{
	std::map<std::string, const FileProducerInterface *>::iterator i;
	for (i = _Producers.begin(); i != _Producers.end(); i++)
	{
		if (i->second == producer)
		{
			PXL_LOG_INFO << "unregister file producer for " << i->first;
			_Producers.erase(i);
			return;
		}
	}
}

} // namespace pxl

