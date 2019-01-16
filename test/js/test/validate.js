import {expect} from 'chai'
import chai from 'chai'
import chaiSubset from 'chai-subset'
import child_process from 'child_process'
import fs from 'fs'
import https from 'https'
import path from 'path'
import Promise from 'bluebird'
import tmp from 'tmp'
import validator from 'gltf-validator'

chai.use(chaiSubset)

const execPromise = Promise.promisify(child_process.execFile)

const SAMPLE_MODELS_BASE_URL = 'https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/sourceModels/'
const COLLADA2GLTF_PATH = '../../build/COLLADA2GLTF-bin'

async function convert(collada_url) {
    const tmpDir = tmp.dirSync({
        unsafeCleanup: true
    })
    try {
        const dirName = tmpDir.name
        const fileName = collada_url.split('/').pop()
        const targetFileName = path.join(dirName, fileName)
        const targetStream = fs.createWriteStream(targetFileName)
        https.get(collada_url, (res) => {
            res.pipe(targetStream)
        })
        await new Promise((resolve, reject) => {
            targetStream.on('finish', () => {
                targetStream.close()
                resolve()
            })
        })
        const collada = fs.readFileSync(targetFileName, 'UTF-8')
        const stdout = await execPromise(COLLADA2GLTF_PATH, [targetFileName])
        const outputFileName = path.join(dirName, 'output', fileName.replace('.dae', '.gltf'))
        return fs.readFileSync(outputFileName)
    } finally {
        tmpDir.removeCallback()
    }
}

async function validate(asset, expected) {
    const report = await validator.validateBytes(new Uint8Array(asset))
    expect(report).to.containSubset(expected)
    return report
}

describe('Test and validate model conversions', () => {
    it('AnimatedMorphCube', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'AnimatedMorphCube/AnimatedMorphCube.dae')
        await validate(asset, {
            info: {
                hasAnimations: true,
                hasMorphTargets: true,
                primitivesCount: 1
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('Box', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'Box/Box.dae')
        await validate(asset, {
            info: {
                primitivesCount: 1
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('BoxAnimated', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'BoxAnimated/BoxAnimated.dae')
        await validate(asset, {
            info: {
                hasAnimations: true,
                primitivesCount: 2
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('BoxTextured', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'BoxTextured/BoxTextured.dae')
        await validate(asset, {
            info: {
                hasTextures: true,
                primitivesCount: 1,
                resources: [{
                    uri: 'CesiumLogoFlat.png'
                }]
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('BrainStem', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'BrainStem/BrainStem.dae')
        await validate(asset, {
            info: {
                hasAnimations: true,
                hasSkins: true,
                primitivesCount: 59
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    }).timeout(10000)

    it('Buggy', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'Buggy/Buggy.dae')
        await validate(asset, {
            info: {
                primitivesCount: 148
            }, issues: {
                numErrors: 0,
                /**
                 * Buggy has degenerate triangles which generates infos.
                 * Once we optimize them out, this should be uncommented.
                 */
                //numInfos: 0,
                numWarnings: 0
            }
        })
    }).timeout(10000)

    it('CesiumMan', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'CesiumMan/CesiumMan.dae')
        await validate(asset, {
            info: {
                hasAnimations: true,
                hasSkins: true,
                hasTextures: true,
                primitivesCount: 1,
                resources: [{
                    uri: 'CesiumMan.jpg'
                }]
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('CesiumMilkTruck', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'CesiumMilkTruck/CesiumMilkTruck.dae')
        await validate(asset, {
            info: {
                hasAnimations: true,
                hasTextures: true,
                primitivesCount: 4,
                resources: [{
                    uri: 'CesiumMilkTruck.png'
                }]
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('Duck', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'Duck/Duck.dae')
        await validate(asset, {
            info: {
                hasTextures: true,
                primitivesCount: 1,
                resources: [{
                    uri: 'DuckCM.png'
                }]
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('GearboxAssy', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'GearboxAssy/GearboxAssy.dae')
        await validate(asset, {
            info: {
                primitivesCount: 42
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    }).timeout(10000)

    it('Monster', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'Monster/Monster.dae')
        await validate(asset, {
            info: {
                hasAnimations: true,
                hasSkins: true,
                hasTextures: true,
                primitivesCount: 1,
                resources: [{
                    uri: 'Monster.jpg'
                }]
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('ReciprocatingSaw', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'ReciprocatingSaw/ReciprocatingSaw.dae')
        await validate(asset, {
            info: {
                primitivesCount: 65
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    }).timeout(10000)

    it('RiggedFigure', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'RiggedFigure/RiggedFigure.dae')
        await validate(asset, {
            info: {
                hasAnimations: true,
                hasSkins: true,
                primitivesCount: 1,
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('RiggedSimple', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'RiggedSimple/RiggedSimple.dae')
        await validate(asset, {
            info: {
                hasAnimations: true,
                hasSkins: true,
                primitivesCount: 1,
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('VC', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'VC/VC.dae')
        await validate(asset, {
            info: {
                hasAnimations: true,
                hasTextures: true,
                primitivesCount: 167,
                resources: [{
                    uri: '001.jpg'
                }, {
                    uri: 'cockpit-map.jpg'
                }, {
                    uri: 's_08.jpg'
                }, {
                    uri: 's_06.jpg'
                }, {
                    uri: 's_04.jpg'
                }, {
                    uri: 's_02.jpg'
                }, {
                    uri: 's_07.jpg'
                }, {
                    uri: 's_03.jpg'
                }, {
                    uri: 's_05.jpg'
                }, {
                    uri: 's_01.jpg'
                }, {
                    uri: '002.jpg'
                }, {
                    uri: '11.jpg'
                },, {
                    uri: 'machine.jpg'
                }, {
                    uri: 'prop128.png'
                }, {
                    uri: 'scrapsurf03-red.jpg'
                }, {
                    uri: 'f22.jpg'
                }, {
                    uri: 'heli.jpg'
                }, {
                    uri: 'O21.jpg'
                }, {
                    uri: '5.jpg'
                }, {
                    uri: 'surface01.jpg'
                }]
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })

    it('WalkingLady', async () => {
        const asset = await convert(SAMPLE_MODELS_BASE_URL + 'WalkingLady/WalkingLady.dae')
        await validate(asset, {
            info: {
                hasAnimations: true,
                hasSkins: true,
                primitivesCount: 1
            }, issues: {
                numErrors: 0,
                numInfos: 0,
                numWarnings: 0
            }
        })
    })
})