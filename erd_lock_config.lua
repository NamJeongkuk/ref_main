return {
  common_erds = {
    error_message = 'You have changed a common ERD to all appliances. Please revert your changes.',
    erds = {
      '0x0001',
      '0x0002',
      '0x0008',
      '0x0030',
      '0x0031',
      '0x0032',
      '0x0035',
      '0x0038',
      '0x0039',
      '0x003A',
      '0x003B',
      '0x003C',
      '0x007F',
      '0x008F',
      '0x0090',
      '0x0092'
    }
  },
  production = {
    error_message = 'Do not change these ERDs. Adding values to an enumeration or giving meaning to unused fields is valid. Make sure to update the erd-specification repo.',
    erds = {
      '0x0700',
      '0x0701',
      '0x0702',
      '0x0703'
    }
  },
  titanic_ui = {
    error_message = 'Do not change these ERDs. If they change, Titanic UI\'s Common ERDs need updated.',
    erds = {
      '0xF729',
      '0xF72A',
      '0xF72B',
      '0xF72C',
      '0xFAFB',
      '0xFAFC',
      '0xFAFD',
      '0xFAFE'
    }
  },
  refrigeration_public_erds = {
    error_message = 'Do not change these ERDs. If they change, please check with the public ERD spec page and create tickets to make the changes on other boards that use these ERDs',
    erds = {
      '0x1200',
      '0x1201',
      '0x1202',
      '0x1203',
      '0x1204',
      '0x1205',
      '0x1206',
      '0x1207',
      '0x1208',
      '0x1209',
      '0x120A',
      '0x120B',
      '0x120C',
      '0x120D',
      '0x120E',
      '0x120F',
      '0x1210',
      '0x1211',
      '0x1212',
      '0x1213',
      '0x1215',
      '0x1216',
      '0x1217',
      '0x1218',
      '0x1219',
      '0x121A',
      '0x121B',
      '0x1227',
      '0x1228',
      '0x1233'
    }
  },
  pqa_delta_t_sequences_erds = {
    error_message = 'Do not change these ERDs. The ERDs are used by PQA and Delta T Sequences.',
    erds = {
      '0x0035',
      '0x003A',
      '0x003B',
      '0x120E',
      '0x120F',
      '0xF0C0',
      '0xF0C1',
      '0xF0C2',
      '0xF0C5',
      '0xF140',
      '0xF202',
      '0xF22C',
      '0xF2BC',
      '0xF2D4',
      '0xF2E1',
      '0xF31C',
      '0xF342',
      '0xF44D',
      '0xF44E',
      '0xF50B',
      '0xF52A',
      '0xF52E',
      '0xF56F',
      '0xF574',
      '0xF575',
      '0xF576',
      '0xF577',
      '0xF612',
      '0xF613',
      '0xF621',
      '0xF622',
      '0xF623',
      '0xF627',
      '0xF638',
      '0xF801',
      '0xFB86'
    }
  }
}
