const ErdInfoSphere = require('erd-core').ErdInfoSphere;
const fs = require('fs');
const path = require('path');

module.exports = () => {
   const infoSphere = ErdInfoSphere();

   const jsonDirectory = path.join(__dirname, '../../../../build/rockhopper/doc/');
   try {
      fs.readdirSync(jsonDirectory).forEach(file => {
         if (file.endsWith('.json')) {
            infoSphere.addDefinitions(JSON.parse(fs.readFileSync(path.join(jsonDirectory, file))));
         }
      });
   } catch (e) {
      throw 'Could not access build/rockhopper/doc, ensure that the folder exists and contains .json files';
   }
   return infoSphere;
};
