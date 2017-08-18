for sampleModel in $( ls -d ~/gltf-test/sampleModels/*/ ); do
  echo $sampleModel
  ls -la $sampleModel/glTF/*.bin
  ls -la $sampleModel/glTF-Draco/*.bin
done
