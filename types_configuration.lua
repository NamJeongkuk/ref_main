return {
  -- Types that will show up in the preprocessed header but don't need to be
  -- defined correctly (ie: they won't actually be in any ERDs)
  ignored_types = {
    'Timer_t',
    'max_align_t',
    'KeyErdStreamData_t',
    'StreamedErd_t'
  },

  -- Types that can't be automatically generated but need to be defined
  type_overrides = {
  }
}
