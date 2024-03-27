//
// Created by dominiq on 3/20/24.
//

#include "TextureAtlas.h"
#include <fstream>


// Loads a Moony Texture Pack File (.mtpf) to populate the atlas table.
bool moony::TextureAtlas::loadFromFile(const std::string& file_path){
    std::ifstream input_file(file_path, std::ifstream::in | std::ifstream::binary);

    if(!input_file)
	return false;

    std::string path = file_path.substr(0, file_path.find_last_of("\\/") + 1);
    std::string input;

    while(input_file >> input){
	m_atlas_list.emplace_back();

	if(input == "D"){
#ifdef USE_ZLIB
	    sf::Vector2u img_size;
	    auto comp_size = std::streamsize();

	    (input_file >> img_size.x >> img_size.y >> comp_size).ignore(1);

	    unsigned long data_size = img_size.x * img_size.y * 4;
	    auto data = new Bytef[data_size];

	    input.resize(comp_size);
	    input_file.read(&input[0], comp_size);
	    uncompress(data, &data_size, reinterpret_cast<Bytef*>(&input[0]), comp_size);

	    m_atlas_list.back().m_texture->create(img_size.x, img_size.y);
	    m_atlas_list.back().m_texture->update((sf::Uint8*)data);

	    delete data;
#endif
	} else {
	    size_t name_len;
	    (input_file >> name_len).ignore(1);
	    input.resize(name_len);
	    input_file.read(&input[0], static_cast<int>(name_len));

	    m_atlas_list.back().m_texture->loadFromFile(path + input);
	}

	size_t img_count;
	input_file >> img_count;

	for(size_t index = 0; index < img_count; index++){
		size_t name_len;
		(input_file >> name_len).ignore(1);
		input.resize(name_len);
		input_file.read(&input[0], static_cast<int>(name_len));

		sf::IntRect texture_rect;
		input_file >> texture_rect.left >> texture_rect.top >> texture_rect.width >> texture_rect.height;

		m_atlas_list.back().m_texture_table[input] = texture_rect;
	}
    }

    input_file.close();

    if(m_atlas_list.empty())
	    return false;

    return true;
}

moony::Texture moony::TextureAtlas::findSubTexture(const std::string& name){

    for(auto& atlas : m_atlas_list)
	if(atlas.m_texture_table.contains(name))
	    return Texture(atlas.m_texture.get(), atlas.m_texture_table[name]);

    return Texture();
}

// This function gets a list of all the seperate image names that are in the TextureAtlas.
// Returns a std::vector<std::string> full of filenames corresponding to images in the TextureAtlas.
std::vector<std::string> moony::TextureAtlas::getSubTextureNames() const{
    std::vector<std::string> names;

    for(const auto& atlas : m_atlas_list)
	for(const auto& [string, rect] : atlas.m_texture_table)
	    names.push_back(string);

    return names;
}
