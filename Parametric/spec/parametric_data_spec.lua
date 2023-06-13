local ParametricData = require 'ParametricData'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local git_hash = require 'GitHash/GitHash'

local function mock_git_hash_short()
  return { 0x66, 0x43, 0xbf, 0x6f }
end

git_hash.short = mock_git_hash_short

describe('parametric_data', function()
  local parametric_data = ParametricData(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      major_data_structure_version = 1,
      minor_data_structure_version = 2,
      major_version = 3,
      minor_version = 4,
      personalities = {
        TypedString('parametric_personality', 'personality_0'),
        TypedString('parametric_personality', 'personality_1'),
        TypedString('parametric_personality', 'personality_2'),
        TypedString('parametric_personality', 'personality_3'),
        TypedString('parametric_personality', 'personality_4'),
        TypedString('parametric_personality', 'personality_5'),
        TypedString('parametric_personality', 'personality_6'),
        TypedString('parametric_personality', 'personality_7'),
        TypedString('parametric_personality', 'personality_8'),
      }
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(parametric_data, generate_config())
  end)

  it('should assert if arguments are wrong', function()
    should_fail_with('major_data_structure_version=-1 must be in [0, 255]', function()
      parametric_data(generate_config({
        major_data_structure_version = -1
      }))
    end)

    should_fail_with('minor_data_structure_version=-1 must be in [0, 255]', function()
      parametric_data(generate_config({
        minor_data_structure_version = -1
      }))
    end)

    should_fail_with('major_version=-1 must be in [0, 255]', function()
      parametric_data(generate_config({
        major_version = -1
      }))
    end)

    should_fail_with('minor_version=-1 must be in [0, 255]', function()
      parametric_data(generate_config({
        minor_version = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type parametric_data', function()
    local expected = remove_whitespace([[
    <?xmlversion="1.0"encoding="utf-8"?>
    <ParametricDataxmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"xsi:noNamespaceSchemaLocation="apl.xsd">
      <Metadata>
        <Endianess>LittleEndian</Endianess>
        <PointerSize>4</PointerSize>
        <ByteAlignment>4</ByteAlignment>
        <Filler>FF</Filler>
        <BaseAddress>FFFF5000</BaseAddress>
        <SizeTSize>4</SizeTSize>
      </Metadata>
      <Data>
        <Structure
          name="Header"><U16
          name="ImageCRC">0</U16><U16
          name="HeaderCRC">0</U16><U8
          name="VersionMajor">1</U8><U8
          name="VersionMinor">2</U8><U8
          name="VersionBuild">3</U8><U8
          name="VersionRevision">4</U8><U16
          name="ApplicationID">313</U16><U16
          name="ParametricDataID">313</U16><U16
          name="AuxiliaryID">313</U16><U16
          name="HardwareID">1</U16><U8
          name="ImageType">3</U8><U8
          name="Filler0">0</U8><U8
          name="LocalParametricDataSize">0</U8><U8
          name="LocalParametricDataCount">0</U8><U32
          name="LocalParametricDataStartAddress">0</U32><U32
          name="ExecutionStartAddress">0</U32><U8
          name="Filler1">0</U8><U8
          name="Filler2">0</U8><U8
          name="Filler3">0</U8><U8
          name="PageCount">1</U8>
          <Structure
            name="Addresses">
            <Structure
              name="Page0"><U32
              name="Start">4294922240</U32><U32
              name="End">4294946815</U32>
            </Structure>
          </Structure>
        </Structure>
        structure(
         structure(
           u8(102),
           u8(67),
           u8(191),
           u8(111)
         ),
          u8(9),
          pointer(structure(
            pointer(personality_0),
            pointer(personality_1),
            pointer(personality_2),
            pointer(personality_3),
            pointer(personality_4),
            pointer(personality_5),
            pointer(personality_6),
            pointer(personality_7),
            pointer(personality_8)
          ))
        )
        attic()
      </Data>
    </ParametricData>]])

    local actual = parametric_data(generate_config())
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)
end)
