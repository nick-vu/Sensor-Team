% This script takes tables of HSL data and plots them in 3D space
% Input format is 1 column of newline separated hue, saturation, and
% lightness values

redMat = table2array(red);
greenMat = table2array(green);
blueMat = table2array(blue);
yellowMat = table2array(yellow);
whiteMat = table2array(white);
carpetMat = table2array(carpet);

colorCell = {redMat, greenMat, blueMat, yellowMat, whiteMat, carpetMat};

% convert data so it's plotted correctly in 3D
for n = 1:length(colorCell)
    color = colorCell{n};
    % put the data in the matrix correctly -> X by 3
    color = [color(1:3:end)'; color(2:3:end)'; color(3:3:end)']';
    
    % x = s*cos(h), y = s*sin(h), z = l
    colorOut = color;
    colorOut(:, 1) = color(:, 2) .* cosd(color(:, 1));
    colorOut(:, 2) = color(:, 2) .* sind(color(:, 1));
    colorOut(:, 3) = color(:, 3)
    % put it back in the cell array
    colorCell{n} = colorOut;
end

% plot all the points in 3D space with corresponding colored points
x = [];
y = [];
z = [];
c = []; % colors
colorMap = {[1 0 0], [0 1 0], [0 0 1], [1 1 0], [1 1 1], [0 0 0]};
s = 25;
for n = 1:length(colorCell)
    x = [x; colorCell{n}(:, 1)];
    y = [y; colorCell{n}(:, 2)];
    z = [z; colorCell{n}(:, 3)];
    c = [c; repmat(colorMap{n}, numel(colorCell{n}(:, 1)), 1)];
end

% plot circles on upper and lower limits of lightness (z axis)
hold on
th = 0:pi/50:2*pi;
xunit = cos(th);
yunit = sin(th);
zunit = vec2mat(ones(1, numel(th)), numel(th));
plot(xunit, yunit);
plot3(xunit, yunit, zunit);
xlabel('x');
ylabel('y');
zlabel('z');

scatter3(x, y, z, s, c, 'filled', 'MarkerEdgeColor', 'k')
hold off