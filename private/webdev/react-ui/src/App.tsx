import './App.css';
import React, {Component, useRef, useState,} from 'react';

import {Canvas, useFrame} from '@react-three/fiber';
import {SOSO11} from '@cocr/ml/src/soso11';

const CANVAS_IN = 'canvasInput';
const CANVAS_OUT = 'canvasOutput';
const enable3D = false;

export default class App extends Component {
    constructor(props) {
        super(props);
    }

    render() {
        const direction = window.innerWidth > window.innerHeight ? 'row' : 'column';
        const img = new Image();
        img.onload = () => {
            let canvas = document.getElementById(CANVAS_IN);
            if (null === canvas) return;
            canvas.width = img.width;
            canvas.height = img.height;
            const ctx = canvas.getContext('2d');
            ctx.drawImage(img, 0, 0, canvas.width, canvas.height);
        }
        const drawBoxes = (boxes) => {
            const inputCanvas = document.getElementById(CANVAS_IN);
            const outputCanvas = document.getElementById(CANVAS_OUT);
            if (!inputCanvas || !outputCanvas) return;
            outputCanvas.width = inputCanvas.width;
            outputCanvas.height = inputCanvas.height;
            let ctx = outputCanvas.getContext('2d');
            ctx.drawImage(img, 0, 0, outputCanvas.width, outputCanvas.height);
            ctx.font = 'bold 15px Arial';
            ctx.lineWidth = 1;
            let arr = [0, 50, 100, 150, 200, 250, 255, 10, 80, 123, 182, 109];
            for (let i = 0; i < boxes.length; i++) {
                let e = boxes[i], id = e.id;
                ctx.strokeStyle = 'rgb(' + (255 - arr[id]) + ',' + (100 - arr[id]) + ',' + (arr[id]) + ')';
                ctx.beginPath();
                ctx.fillText(String(i), e.getCent()[0], e.getCent()[1], 30);
                if (e.isBond()) {
                    ctx.arc(e.startX, e.startY, 4, 0, 180);
                    ctx.fillStyle = 'black';
                    ctx.fill();
                    ctx.stroke();
                }
                ctx.rect(e.startX, e.startY, e.endX - e.startX, e.endY - e.startY);
                ctx.stroke();
            }
        };
        return (
            <div>
                {!enable3D ? null : <Canvas>
                    <ambientLight/>
                    <pointLight position={[10, 10, 10]}/>
                    <Box position={[-1.2, 0, 0]}/>
                    <Box position={[1.2, 0, 0]}/>
                </Canvas>}
                <div style={{display: 'flex', flexDirection: direction}}>
                    <div style={{border: '2px solid green', margin: '10px'}}>
                        <input type={'file'} id={'file'} onChange={(e) => {
                            if (e?.target?.files?.length < 1) return;
                            const file = e.target.files[0];
                            const reader = new FileReader();
                            reader.onload = () => {
                                if (typeof reader.result === 'string') {
                                    img.src = reader.result;
                                }
                            }
                            reader.readAsDataURL(file);
                        }}/>
                        <p>{'输入：'}</p>
                        <canvas id={CANVAS_IN}/>
                    </div>
                    <div style={{border: '2px solid red', margin: '10px'}}>
                        <input type={'button'} value={'推理：'} onClick={() => {
                            SOSO11.predict(CANVAS_IN, './model.json').then(
                                (result) => drawBoxes(result));
                        }}/>
                        <p>{'输出：'}</p>
                        <canvas id={CANVAS_OUT}/>
                    </div>
                </div>
            </div>
        );
    }
}

function Box(props) {
    // This reference will give us direct access to the mesh
    const mesh = useRef();
    // Set up state for the hovered and active state
    const [hovered, setHover] = useState(false);
    const [active, setActive] = useState(false);
    // Rotate mesh every frame, this is outside of React without overhead
    useFrame(() => (mesh.current.rotation.x += 0.01));
    return (
        <mesh
            {...props}
            ref={mesh}
            scale={active ? 1.5 : 1}
            onClick={(event) => setActive(!active)}
            onPointerOver={(event) => setHover(true)}
            onPointerOut={(event) => setHover(false)}>
            <boxGeometry args={[1, 2, 3]}/>
            <meshStandardMaterial color={hovered ? 'hotpink' : 'orange'}/>
        </mesh>
    )
}
