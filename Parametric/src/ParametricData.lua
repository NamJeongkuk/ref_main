return function(core)
  local parametric_data = require 'lua-parametric-tools'.common.parametric_data
  local image_type = require 'lua-parametric-tools'.image.type
  local import = require 'lua-common'.utilities.import
  local constraint = require 'lua-common'.utilities.constraint
  local validate_arguments = require 'lua-common'.utilities.validate_arguments
  local header = require 'lua-parametric-tools'.image.header
  local start_address = 0xFFFEF000
  local git_hash = require 'GitHash/GitHash'
  import(core)

  return function(config)
    validate_arguments(config, {
      major_data_structure_version = { constraint.u8 },
      minor_data_structure_version = { constraint.u8 },
      major_version = { constraint.u8 },
      minor_version = { constraint.u8 },
      personalities = {
        constraint.array_size({ constraint.in_range(1, 254) }),
        constraint.array_elements({ constraint.typed_string('parametric_personality') })
      }
    })

    local personality_array = {}
    for _, personality in ipairs(config.personalities) do
      table.insert(personality_array, core.pointer(personality))
    end

    local git_hash_array = {}
    for _, byte in ipairs(git_hash.short()) do
      table.insert(git_hash_array, core.u8(byte))
    end

    return parametric_data({
      endianness = 'LittleEndian',
      pointer_size = 4,
      byte_alignment = 4,
      filler = 255,
      base_address = start_address,
      data = table.concat({
        header({
          version = {
            crit_major = config.major_data_structure_version,
            crit_minor = config.minor_data_structure_version,
            major = config.major_version,
            minor = config.minor_version,
          },
          hardware_id = 1,
          image_id = 0x0139,
          image_type = image_type.parametric,
          pages = {
            {
              start_address = start_address,
              end_address = 0xFFFFAFFF
            }
          }
        }),
        structure(
          structure(table.unpack(git_hash_array)),
          u8(#config.personalities),
          pointer(structure(
            table.unpack(personality_array)
          ))
        ),
        core.attic()
      })
    })
  end
end
