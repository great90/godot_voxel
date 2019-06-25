#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "../math/vector3i.h"
#include <core/os/dir_access.h>
#include <core/os/file_access.h>

inline Vector3i get_vec3u8(FileAccess *f) {
	Vector3i v;
	v.x = f->get_8();
	v.y = f->get_8();
	v.z = f->get_8();
	return v;
}

inline void store_vec3u8(FileAccess *f, const Vector3i v) {
	f->store_8(v.x);
	f->store_8(v.y);
	f->store_8(v.z);
}

inline Vector3i get_vec3u32(FileAccess *f) {
	Vector3i v;
	v.x = f->get_32();
	v.y = f->get_32();
	v.z = f->get_32();
	return v;
}

inline void store_vec3u32(FileAccess *f, const Vector3i v) {
	f->store_32(v.x);
	f->store_32(v.y);
	f->store_32(v.z);
}

inline Error check_magic_and_version(FileAccess *f, uint8_t expected_version, const char *expected_magic, uint8_t &out_version) {

	uint8_t magic[5] = { '\0' };
	int count = f->get_buffer(magic, 4);
	if (count != 4) {
		ERR_PRINT("Unexpected end of file");
		return ERR_FILE_CORRUPT;
	}
	for (int i = 0; i < 4; ++i) {
		if (magic[i] != expected_magic[i]) {
			ERR_PRINT("Invalid magic");
			return ERR_FILE_CORRUPT;
		}
	}

	out_version = f->get_8();
	if (out_version != expected_version) {
		ERR_PRINT("Invalid version");
		return ERR_FILE_CORRUPT;
	}

	return OK;
}

inline Error check_directory_created(const String &directory_path) {

	DirAccess *d = DirAccess::create_for_path(directory_path);

	if (d == nullptr) {
		ERR_PRINT("Could not access to filesystem");
		return ERR_FILE_CANT_OPEN;
	}

	if (!d->exists(directory_path)) {
		// Create if not exist
		Error err = d->make_dir_recursive(directory_path);
		if (err != OK) {
			ERR_PRINT("Could not create directory");
			memdelete(d);
			return err;
		}
	}

	memdelete(d);
	return OK;
}

struct AutoDeleteFile {
	FileAccess *f = nullptr;
	~AutoDeleteFile() {
		CRASH_COND(f == nullptr);
		memdelete(f);
	}
};

inline FileAccess *open_file(const String &path, FileAccess::ModeFlags mode, Error &out_error) {

	FileAccess *fa = FileAccess::create_for_path(path);
	if (fa == nullptr) {
		out_error = ERR_FILE_NO_PERMISSION;
		return nullptr;
	}

	// Godot does NOT return FILE_NOT_FOUND if the file doesnt exist, so I tried to workaround it.
	// Unfortunately, `exist()` just calls `open()` anyways... back to square one.
	//
	//	if (!fa->exists(path)) {
	//		out_error = ERR_FILE_NOT_FOUND;
	//		memdelete(fa);
	//		return nullptr;
	//	}

	FileAccess *f = fa->open(path, mode, &out_error);
	memdelete(fa);

	return f;
}

#endif // FILE_UTILS_H