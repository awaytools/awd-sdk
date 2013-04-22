package
{
	import away3d.animators.SkeletonAnimator;
	import away3d.arcane;
	import away3d.containers.View3D;
	import away3d.controllers.HoverController;
	import away3d.entities.Mesh;
	import away3d.events.AssetEvent;
	import away3d.events.LoaderEvent;
	import away3d.library.assets.AssetType;
	import away3d.lights.DirectionalLight;
	import away3d.lights.PointLight;
	import away3d.loaders.Loader3D;
	import away3d.loaders.parsers.AWD2Parser;
	import away3d.materials.ColorMaterial;
	import away3d.materials.TextureMaterial;
	import away3d.materials.lightpickers.StaticLightPicker;
	
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.geom.Vector3D;
	import flash.net.URLRequest;
	
	use namespace arcane;
	
	[SWF(width="800", height="600")]
	public class AWDViewer extends Sprite
	{
		private var _view : View3D;
		private var _loader : Loader3D;
		
		private var _hoverCtrl : HoverController;
		private var _prevMouseX : Number;
		private var _prevMouseY : Number;
		
		//private var _skeleton : Skeleton;
		private var _animator : SkeletonAnimator;
		
		private var _defDirLight : DirectionalLight;
		private var _defCamLight : PointLight;
		private var _defLightPicker : StaticLightPicker;
		
		public function AWDViewer()
		{
			init();
			load(loaderInfo.parameters['awd'] || 'default.awd');
			
			// Force resize
			stage.dispatchEvent(new Event(Event.RESIZE));
		}
		
		
		private function init() : void
		{
			_view = new View3D();
			_view.antiAlias = 4;
			_view.camera.lens.far = 5000;
			_view.backgroundColor = parseInt(loaderInfo.parameters['bgcolor'],16) || 0;
			addChild(_view);
			addEventListener(Event.ENTER_FRAME, onEnterFrame);
			
			_hoverCtrl = new HoverController(_view.camera);
			_hoverCtrl.tiltAngle = 0;
			_hoverCtrl.panAngle = 180;
			_hoverCtrl.minTiltAngle = -60;
			_hoverCtrl.maxTiltAngle = 60;
			
			_defDirLight = new DirectionalLight(1, -3, 1);
			_defDirLight.specular = 0.1;
			_defCamLight = new PointLight();
			_defCamLight.specular = 0.1;
			_defLightPicker = new StaticLightPicker([_defDirLight,_defCamLight]);
			
			stage.addEventListener(MouseEvent.MOUSE_DOWN, onStageMouseDown);
			stage.addEventListener(MouseEvent.MOUSE_MOVE, onStageMouseMove);
			stage.addEventListener(MouseEvent.MOUSE_WHEEL, onStageMouseWheel);
			
			stage.align = StageAlign.TOP_LEFT;
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.addEventListener(Event.RESIZE, onStageResize);
		}
		
		
		private function load(url : String) : void
		{
			Loader3D.enableParser(AWD2Parser);
			_loader = new Loader3D(true);
			_loader.addEventListener(AssetEvent.ASSET_COMPLETE, onAssetComplete);
			_loader.addEventListener(LoaderEvent.RESOURCE_COMPLETE, onResourceComplete);
			_loader.load(new URLRequest(url));
		}
		
		
		private var _i : uint = 0;
		
		private function onAssetComplete(ev : AssetEvent) : void
		{
			if (ev.asset.assetType == AssetType.MESH) {
				var mesh : Mesh;
				var texMat : TextureMaterial;
				
				mesh = Mesh(ev.asset);
				texMat = mesh.material as TextureMaterial;
				
				if (mesh.material == null || (texMat && texMat.texture == null)) {
					mesh.material = new ColorMaterial(Math.random() * 0xffffff);
				}
				
				mesh.material.lightPicker = _defLightPicker;
				
				trace(mesh.position);
			}
			else if (ev.asset.assetType == AssetType.SKELETON) {
				//_skeleton = Skeleton(ev.asset);
			}
			else if (ev.asset.assetType == AssetType.MATERIAL) {
				trace(_i++, ev.asset.name, ev.asset.assetType, ev.asset);
			}
		}
		
		
		private function onResourceComplete(ev : LoaderEvent) : void
		{
			var scale : Number;
			var max : Vector3D;
			var min : Vector3D;
			var d : Vector3D;
			
			/*
			if (_skeleton) {
				var it : AssetLibraryIterator;
				var mesh : Mesh;
				var geom : Geometry;
				var jpv : int;
				
				it = AssetLibrary.createIterator(AssetType.MESH);
				while (mesh = Mesh(it.next())) {
					if (mesh.geometry && mesh.geometry.subGeometries.length > 0) {
						var sub : SkinnedSubGeometry = mesh.geometry.subGeometries[0] as SkinnedSubGeometry;
						if (sub != null) {
							geom = mesh.geometry;
							jpv = sub.jointIndexData.length / sub.numVertices;
							break;
						}
					}
				}
				
				if (geom) {
					var seq : SkeletonAnimationSequence;
					var name : String;
					var state : SkeletonAnimationState;
					
					geom.animation = new SkeletonAnimation(_skeleton, jpv, true);
					state = SkeletonAnimationState(mesh.animationState);
					
					_animator = new SkeletonAnimator(state);
					_animator.updateRootPosition = false;
					
					it = AssetLibrary.createIterator(AssetType.ANIMATION);
					while (seq = SkeletonAnimationSequence(it.next())) {
						_animator.addSequence(seq);
						
						if (!name)
							name = seq.name;
					}
					
					_animator.play(name);
				}
			}
			*/
			
			max = new Vector3D(_loader.maxX, _loader.maxY, _loader.maxZ);
			min = new Vector3D(_loader.minX, _loader.minY, _loader.minZ);
			d = max.subtract(min);
			
			scale = 300 / d.length;
			_loader.scale(scale);
			
			_view.scene.addChild(_loader);
			
			// Force resize
			stage.dispatchEvent(new Event(Event.RESIZE));
		}
		
		
		private function onStageMouseDown(ev : MouseEvent) : void
		{
			_prevMouseX = ev.stageX;
			_prevMouseY = ev.stageY;
		}
		
		private function onStageMouseMove(ev : MouseEvent) : void
		{
			if (ev.buttonDown) {
				_hoverCtrl.panAngle += (ev.stageX - _prevMouseX);
				_hoverCtrl.tiltAngle += (ev.stageY - _prevMouseY);
			}
			
			_prevMouseX = ev.stageX;
			_prevMouseY = ev.stageY;
		}
		
		private function onStageMouseWheel(ev : MouseEvent) : void
		{
			_hoverCtrl.distance -= ev.delta * 5;
			
			if (_hoverCtrl.distance < 5)
				_hoverCtrl.distance = 5;
			else if (_hoverCtrl.distance > 2000)
				_hoverCtrl.distance = 2000;
		}
		
		
		private function onStageResize(ev : Event) : void
		{
			_view.width = stage.stageWidth;
			_view.height = stage.stageHeight;
		}
		
		
		private function onEnterFrame(ev : Event) : void
		{
			_hoverCtrl.update();
			
			_defCamLight.x = _view.camera.x;
			_defCamLight.y = _view.camera.y;
			_defCamLight.z = _view.camera.z;
			
			_view.render();
		}
	}
}