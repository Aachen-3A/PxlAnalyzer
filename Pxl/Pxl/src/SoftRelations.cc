//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "Pxl/Pxl/interface/pxl/core/SoftRelations.hh"

#include "Pxl/Pxl/interface/pxl/core/Relative.hh"
#include "Pxl/Pxl/interface/pxl/core/ObjectOwner.hh"
#include "Pxl/Pxl/interface/pxl/core/BasicContainer.hh"

namespace pxl
{

void SoftRelations::serialize(const OutputStream &out) const
{
	size_t size = _relationsMap.size();
	out.writeUnsignedInt((unsigned int)size);
	for (const_iterator iter = _relationsMap.begin(); iter
			!=_relationsMap.end(); ++iter)
	{
		out.writeString(iter->first);
		iter->second.serialize(out);
	}
}

void SoftRelations::deserialize(const InputStream &in)
{
	_relationsMap.clear();
	unsigned int size;
	in.readUnsignedInt(size);
	for (size_t i=0; i<size; ++i)
	{
		std::string name;
		in.readString(name);
		Id id(in);
		_relationsMap.insert(std::pair<std::string, Id>(name, id));
	}
}
	
Serializable* SoftRelations::getFirst(const ObjectOwner& owner,
		const std::string& type) const
{
	if (type == "")
	{
		const_iterator found = _relationsMap.begin();
		if (found != _relationsMap.end())
		{
			return owner.getById(found->second);
		}
		else
			return 0;
	}
	else
	{
		const_iterator found = _relationsMap.find(type);
		if (found != _relationsMap.end())
		{
			return owner.getById(found->second);
		}
		else
			return 0;
	}

}

// Gets the relatives, if living in the same basic container
int SoftRelations::getSoftRelatives(std::vector<Serializable*>& vec,
		const BasicContainer& owner, const std::string& type) const
{
	int size = vec.size();

	std::pair<const_iterator, const_iterator> iterators =
			_relationsMap.equal_range(type);
	for (const_iterator iter = iterators.first; iter != iterators.second; ++iter)
	{
		Serializable* relative = owner.getById(iter->second);
		if (relative != 0)
			vec.push_back(relative);
	}

	return vec.size() - size;
}

int SoftRelations::getSoftRelatives(std::vector<Serializable*>& vec,
		const BasicContainer& owner) const
{
	int size = vec.size();
	for (const_iterator iter = _relationsMap.begin(); iter
			!= _relationsMap.end(); ++iter)
	{
		Serializable* relative = owner.getById(iter->second);
		if (relative != 0)
			vec.push_back(relative);
	}
	return vec.size() - size;
}

Serializable* SoftRelations::getFirst(const BasicContainer& owner,
		const std::string& type) const
{
	if (type == "")
	{
		const_iterator found = _relationsMap.begin();
		if (found != _relationsMap.end())
		{
			return owner.getById(found->second);
		}
		else
			return 0;
	}
	else
	{
		const_iterator found = _relationsMap.find(type);
		if (found != _relationsMap.end())
		{
			return owner.getById(found->second);
		}
		else
			return 0;
	}

}

int SoftRelations::getSoftRelatives(std::vector<Serializable*>& vec,
		const ObjectOwner& owner, const std::string& type) const
{
	int size = vec.size();

	std::pair<const_iterator, const_iterator> iterators =
			_relationsMap.equal_range(type);
	for (const_iterator iter = iterators.first; iter != iterators.second; ++iter)
	{
		Serializable* relative = owner.getById(iter->second);
		if (relative != 0)
			vec.push_back(relative);
	}

	return vec.size() - size;
}

int SoftRelations::getSoftRelatives(std::vector<Serializable*>& vec,
		const ObjectOwner& owner) const
{
	int size = vec.size();
	for (const_iterator iter = _relationsMap.begin(); iter
			!= _relationsMap.end(); ++iter)
	{
		Serializable* relative = owner.getById(iter->second);
		if (relative != 0)
			vec.push_back(relative);
	}
	return vec.size() - size;
}


int SoftRelations::keepSoftRelatives(std::vector<Serializable*>& vec) const
{
	std::vector<Serializable*> keepItems;
	for (const_iterator iter = _relationsMap.begin(); iter
			!= _relationsMap.end(); ++iter)
	{
		for (std::vector<Serializable*>::const_iterator itVec = vec.begin(); itVec
				!= vec.end(); ++itVec)
		{
			if ((*itVec)->getId() == iter->second)
				keepItems.push_back(*itVec);
		}
	}
	vec.swap(keepItems);
	return vec.size();
}

int SoftRelations::keepSoftRelatives(std::vector<Serializable*>& vec,
		const std::string& type) const
{
	std::vector<Serializable*> keepItems;
	for (const_iterator iter = _relationsMap.begin(); iter
			!= _relationsMap.end(); ++iter)
	{
		if (iter->first == type)
		{
			for (std::vector<Serializable*>::const_iterator itVec = vec.begin(); itVec
					!= vec.end(); ++itVec)
			{
				if ((*itVec)->getId() == iter->second)
					keepItems.push_back(*itVec);
			}
		}
	}
	vec.swap(keepItems);
	return vec.size();
}

bool SoftRelations::has(const Serializable* relative) const
{
	for (const_iterator iter = _relationsMap.begin(); iter
			!= _relationsMap.end(); ++iter)
	{
		if (relative->getId() == iter->second)
			return true;
	}
	return false;
}

bool SoftRelations::has(const Serializable* relative, const std::string& type) const
{
	std::pair<const_iterator, const_iterator> iterators =
			_relationsMap.equal_range(type);
	for (const_iterator iter = iterators.first; iter != iterators.second; ++iter)
	{
		if (relative->getId() == iter->second)
			return true;
	}
	return false;
}

bool SoftRelations::has(const Id& id) const
{
	for (const_iterator iter = _relationsMap.begin(); iter
			!= _relationsMap.end(); ++iter)
	{
		if (id == iter->second)
			return true;
	}
	return false;
}

bool SoftRelations::has(const Id& id, const std::string& type) const
{
	std::pair<const_iterator, const_iterator> iterators =
			_relationsMap.equal_range(type);
	for (const_iterator iter = iterators.first; iter != iterators.second; ++iter)
	{
		if (id == iter->second)
			return true;
	}
	return false;
}

bool SoftRelations::hasType(const std::string& name) const
{
	if (_relationsMap.count(name) > 0)
		return true;
	return false;
}

void SoftRelations::set(const Serializable* relative, const std::string& type)
{
	_relationsMap.insert(std::pair<std::string, Id>(type, relative->getId()));
}

int SoftRelations::count(const std::string& name) const
{
	return _relationsMap.count(name);
}

void SoftRelations::remove(const Serializable* relative)
{
	for (iterator iter = _relationsMap.begin(); iter != _relationsMap.end(); ++iter)
	{
		if (iter->second == relative->getId())
		{
			_relationsMap.erase(iter);
		}
	}
}

void SoftRelations::remove(const Serializable* relative,
		const std::string& type)
{
	std::pair<iterator, iterator> iterators = _relationsMap.equal_range(type);
	for (iterator iter = iterators.first; iter != iterators.second; ++iter)
	{
		if (iter->second == relative->getId())
		{
			_relationsMap.erase(iter);
		}
	}
}

std::ostream& SoftRelations::print(int level, std::ostream& os, int pan) const
{
	os << "SoftRelations of size " << _relationsMap.size() << "\n";
	for (const_iterator iter = _relationsMap.begin(); iter
			!= _relationsMap.end(); ++iter)
	{
		os << "--> ('" << iter->first << "', " << iter->second << ") \n";
	}
	return os;
}


Serializable* SoftRelations::getByIdHelper(const ObjectOwner& owner, const_iterator iter) const
{
		return owner.getById(iter->second);
}
Serializable* SoftRelations::getByIdHelper(const BasicContainer& owner, const_iterator iter) const
{
		return owner.getById(iter->second);
}
} //namespace pxl
