import pygame
import math
import random

pygame.init()

# =================== WINDOW ===================
WIDTH, HEIGHT = 1000, 700
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Spawner Pattern Sandbox")
clock = pygame.time.Clock()
FPS = 60

# =================== CONSTANTS ===================
DEG2RAD = math.pi / 180
CENTER = pygame.Vector2(400, HEIGHT // 2)
FONT = pygame.font.SysFont("consolas", 16)

# Patterns
CIRCLE, SPIRAL, WAVE, BURST, TARGETING = range(5)
PATTERN_NAMES = ["CIRCLE", "SPIRAL", "WAVE", "BURST", "TARGETING"]

# =================== UI SLIDER ===================
class Slider:
    def __init__(self, x, y, w, minv, maxv, value, label):
        self.rect = pygame.Rect(x, y, w, 6)
        self.min = minv
        self.max = maxv
        self.value = value
        self.label = label
        self.dragging = False

    def handle(self, event):
        if event.type == pygame.MOUSEBUTTONDOWN:
            if self.rect.inflate(0, 10).collidepoint(event.pos):
                self.dragging = True
        if event.type == pygame.MOUSEBUTTONUP:
            self.dragging = False
        if event.type == pygame.MOUSEMOTION and self.dragging:
            t = (event.pos[0] - self.rect.x) / self.rect.w
            t = max(0, min(1, t))
            self.value = self.min + t * (self.max - self.min)

    def draw(self):
        pygame.draw.rect(screen, (80, 80, 80), self.rect)
        knob_x = self.rect.x + (self.value - self.min) / (self.max - self.min) * self.rect.w
        pygame.draw.circle(screen, (220, 220, 220), (int(knob_x), self.rect.centery), 8)
        txt = FONT.render(f"{self.label}: {self.value:.1f}", True, (230, 230, 230))
        screen.blit(txt, (self.rect.x, self.rect.y - 18))


# =================== BULLET ===================
class Bullet:
    def __init__(self, pos, vel, r):
        self.pos = pygame.Vector2(pos)
        self.vel = pygame.Vector2(vel)
        self.r = r
        self.active = True

    def update(self, dt):
        self.pos += self.vel * dt
        if self.pos.length() > 2000:
            self.active = False

    def draw(self):
        pygame.draw.circle(screen, (255, 80, 80), self.pos, self.r)
        pygame.draw.circle(screen, (255, 255, 255), self.pos, self.r + 2, 1)


# =================== SPAWNER ===================
class Spawner:
    def __init__(self):
        self.pattern = CIRCLE
        self.timer = 0
        self.angle = 0

    def spawn(self, bullets, params):
        bc, speed, spread, rot = params
        step = spread / max(1, bc - 1)

        if self.pattern in (CIRCLE, BURST, SPIRAL):
            step = spread / bc

        for i in range(bc):
            if self.pattern == TARGETING:
                mouse = pygame.Vector2(pygame.mouse.get_pos())
                base = math.atan2(mouse.y - CENTER.y, mouse.x - CENTER.x)
                ang = base - spread / 2 * DEG2RAD + i * (spread * DEG2RAD / (bc - 1))
            else:
                ang = (i * step + self.angle) * DEG2RAD

            spd = speed
            if self.pattern == BURST:
                spd += random.uniform(-40, 40)

            if self.pattern == WAVE:
                spd += math.sin(self.angle * DEG2RAD * 2 + i) * 20

            vel = pygame.Vector2(math.cos(ang), math.sin(ang)) * spd
            bullets.append(Bullet(CENTER, vel, 5))

    def update(self, bullets, dt, params, cooldown):
        self.timer += dt
        self.angle += params[3] * dt
        if self.timer >= cooldown:
            self.timer = 0
            self.spawn(bullets, params)


# =================== SETUP ===================
spawner = Spawner()
bullets = []

sliders = [
    Slider(820, 100, 150, 1, 20, 6, "Bullet Count"),
    Slider(820, 150, 150, 50, 300, 150, "Speed"),
    Slider(820, 200, 150, 0, 360, 360, "Spread"),
    Slider(820, 250, 150, 0, 360, 90, "Rotation"),
    Slider(820, 300, 150, 0.05, 2.0, 0.5, "Cooldown"),
]

# =================== MAIN LOOP ===================
running = True
while running:
    dt = clock.tick(FPS) / 1000

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN:
            if pygame.K_1 <= event.key <= pygame.K_5:
                spawner.pattern = event.key - pygame.K_1
                bullets.clear()

        for s in sliders:
            s.handle(event)

    params = (
        int(sliders[0].value),
        sliders[1].value,
        sliders[2].value,
        sliders[3].value,
    )
    cooldown = sliders[4].value

    spawner.update(bullets, dt, params, cooldown)

    for b in bullets:
        b.update(dt)
    bullets[:] = [b for b in bullets if b.active][:1500]

    # ============ DRAW ============
    screen.fill((12, 12, 18))

    pygame.draw.circle(screen, (200, 100, 100), CENTER, 20)
    pygame.draw.circle(screen, (0, 0, 0), CENTER, 14)

    for b in bullets:
        b.draw()

    for s in sliders:
        s.draw()

    title = FONT.render(
        f"Pattern: {PATTERN_NAMES[spawner.pattern]}  (1–5 to change)",
        True, (255, 255, 255)
    )
    screen.blit(title, (20, 20))

    pygame.display.flip()

pygame.quit()
