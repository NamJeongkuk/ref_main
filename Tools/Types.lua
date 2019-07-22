return function(types)
  local global_metatable = getmetatable(_G)

  setmetatable(_G, {
    __newindex = function(_, k, v) types[k] = v end,
    __index = types
  })

AsyncDataSource_FlashBlockGroupMetadata_t = struct {
  uint8_t 'blockSequenceId',
  uint8_t 'driverVersion',
  uint8_t 'clientVersion',
  uint8_t 'action'
}

  types['I_BufferedUart_t *'] = intptr_t
  types['I_Gea2MessageEndpoint_t *'] = intptr_t
  types['I_Action_t *'] = intptr_t
  types['I_DataSource_t *'] = intptr_t

  setmetatable(_G, global_metatable)

  return types
end
