function(lvl, name)

  local CArray(array) = '{' + std.foldl(function(l, r) l + ', ' + r, array[1:], array[0]) + '}';

  local WriteLayer_(layer, ldat) =
    'static const Vector2 size_layer_' + layer + ' = { .x = ' + ldat.width + ', .y = ' + ldat.height + '};' +
    'static char layer_' + layer + '[] = ' + CArray(ldat.data) + ';';

  local FindLayer(layer) = std.filter(function(m) m.name == layer, lvl.layers)[0];

  local EntityToStruct(ent) = '{ .type = bhop_Entity_' + ent.type
                              + ', .origin = (Vector2) { .x = ' + ent.x + ', .y = ' + ent.y + '}'
                              + ', .velocity = (Vector2) { .x = 0, .y = 0 }'
                              + ', .is_fixed = 0'
                              + '}';

  local WriteEntities() =
    local ents = FindLayer('Entities');
    local num_ents = std.toString(std.length(ents.objects) + 1);
    'static const int num_ents = ' + num_ents + ';' +
    'static bhop_Entity entities[' + num_ents + '] =' + CArray(std.map(EntityToStruct, ents.objects)) + ';';

  local WriteLayer(layer) = WriteLayer_(layer, FindLayer(layer));


  '#include "level.h"\n'
  + WriteLayer('Terrain') + WriteLayer('Decor') + WriteEntities()
  + 'bhop_Level level_' + name + ' = { .terrain_size = size_layer_Terrain, .decor_size = size_layer_Decor, .entities_count = num_ents, .terrain = layer_Terrain, .decor = layer_Decor, .entities = entities };'
