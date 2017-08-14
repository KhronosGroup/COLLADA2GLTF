for sampleModel in $( ls -d ~/gltf-test/sampleModels/*/ ); do
  echo $sampleModel
  ls -la $sampleModel/glTF
  ls -la $sampleModel/glTF-Draco
done
